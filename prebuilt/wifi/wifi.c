/*
 * Copyright 2008, The Android Open Source Project
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "hardware_legacy/wifi.h"
#include "libwpa_client/wpa_ctrl.h"

#define LOG_TAG "WifiHW"
#include "cutils/log.h"
#include "cutils/memory.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include "private/android_filesystem_config.h"
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif

static struct wpa_ctrl *ctrl_conn;
static struct wpa_ctrl *monitor_conn;

extern int do_dhcp();
extern int ifc_init();
extern void ifc_close();
extern char *dhcp_lasterror();
extern void get_dhcp_info();
extern int init_module(void *, unsigned long, const char *);
extern int delete_module(const char *, unsigned int);

static char iface[PROPERTY_VALUE_MAX];
// TODO: use new ANDROID_SOCKET mechanism, once support for multiple
// sockets is in

#ifndef WIFI_DRIVER_MODULE_PATH
#define WIFI_DRIVER_MODULE_PATH         "/system/lib/modules/wlan.ko"
#endif
#ifndef WIFI_DRIVER_MODULE_NAME
#define WIFI_DRIVER_MODULE_NAME         "wlan"
#endif
#ifndef WIFI_SDIO_IF_DRIVER_MODULE_PATH
#define WIFI_SDIO_IF_DRIVER_MODULE_PATH         ""
#endif
#ifndef WIFI_SDIO_IF_DRIVER_MODULE_NAME
#define WIFI_SDIO_IF_DRIVER_MODULE_NAME ""
#endif
#ifndef WIFI_DRIVER_MODULE_ARG
#define WIFI_SDIO_IF_DRIVER_MODULE_ARG  ""
#define WIFI_DRIVER_MODULE_ARG          ""
#endif

#ifndef WIFI_FIRMWARE_LOADER
#define WIFI_FIRMWARE_LOADER		""
#endif
#ifndef WIFI_PRE_LOADER
#define WIFI_PRE_LOADER		""
#endif
#define WIFI_TEST_INTERFACE		"sta"

#define WIFI_DRIVER_LOADER_DELAY	1000000

static const char IFACE_DIR[]           = "/data/misc/wifi/wpa_supplicant";
static const char DRIVER_MODULE_NAME[]  = WIFI_DRIVER_MODULE_NAME;
static const char DRIVER_SDIO_IF_MODULE_NAME[]  = WIFI_SDIO_IF_DRIVER_MODULE_NAME;
static const char DRIVER_MODULE_TAG[]   = WIFI_DRIVER_MODULE_NAME " ";
static const char DRIVER_MODULE_PATH[]  = WIFI_DRIVER_MODULE_PATH;
static const char DRIVER_SDIO_IF_MODULE_PATH[]  = WIFI_SDIO_IF_DRIVER_MODULE_PATH;
static const char DRIVER_MODULE_ARG[]   = WIFI_DRIVER_MODULE_ARG;
static const char DRIVER_SDIO_IF_MODULE_ARG[]   = WIFI_SDIO_IF_DRIVER_MODULE_ARG;
static const char FIRMWARE_LOADER[]     = WIFI_FIRMWARE_LOADER;
static const char DRIVER_PROP_NAME[]    = "wlan.driver.status";
static const char SUPPLICANT_NAME[]     = "wpa_supplicant";
static const char SUPP_PROP_NAME[]      = "init.svc.wpa_supplicant";
static const char SUPP_CONFIG_TEMPLATE[]= "/system/etc/wifi/wpa_supplicant.conf";
static const char SUPP_CONFIG_FILE[]    = "/data/misc/wifi/wpa_supplicant.conf";
static const char MODULE_FILE[]         = "/proc/modules";
static const char SDIO_POLLING_ON[]     = "/etc/init.qcom.sdio.sh 1";
static const char SDIO_POLLING_OFF[]    = "/etc/init.qcom.sdio.sh 0";
static const char LOCK_FILE[]           = "/data/misc/wifi/drvr_ld_lck_pid";
static const char PRELOADER[]           = WIFI_PRE_LOADER;

static const char AP_DRIVER_MODULE_NAME[]  = "tiap_drv";
static const char AP_DRIVER_MODULE_TAG[]   = "tiap_drv" " ";
static const char AP_DRIVER_MODULE_PATH[]  = "/system/lib/modules/tiap_drv.ko";
static const char AP_DRIVER_MODULE_ARG[]   = "";
static const char AP_FIRMWARE_LOADER[]     = "wlan_ap_loader";
static const char AP_DRIVER_PROP_NAME[]    = "wlan.ap.driver.status";
static int _wifi_unload_driver();   /* Does not check Bluetooth status */

#ifdef WIFI_EXT_MODULE_NAME
static const char EXT_MODULE_NAME[] = WIFI_EXT_MODULE_NAME;
#ifdef WIFI_EXT_MODULE_ARG
static const char EXT_MODULE_ARG[] = WIFI_EXT_MODULE_ARG;
#else
static const char EXT_MODULE_ARG[] = "";
#endif
#endif
#ifdef WIFI_EXT_MODULE_PATH
static const char EXT_MODULE_PATH[] = WIFI_EXT_MODULE_PATH;
#endif
#define MAX_LOCK_TRY    14
#define LOCK_TRY_LAST_CHANCE    2

static int lock(void)
{
    int fd;
    int i = MAX_LOCK_TRY;
    char pid_buf[12];   /* 32-bit decimal pid, \n, \0 */
    int sleep_time;
    int first_pid = -1;

    while (((fd = open(LOCK_FILE, O_EXCL | O_CREAT | O_RDWR, 0660)) < 0)
            && (i > 0)) {

        LOGV("lock file excl open error: %s; cnt: %d", strerror(errno), i);
        sleep_time = 500000;    /* Time till try again (usec) */
        if (errno != EEXIST) {
            LOGE("Can't create lock file: %s", strerror(errno));
            break;
        } else {
            /* After first try, check if the owner exists; delete file if not.
             * If owner does exist, check on last try if owner is still the same
             * and delete file if so (due to stale file or owner is stuck).
             * Else, give the new owner a chance.
             */
            if ((i == MAX_LOCK_TRY) || i == LOCK_TRY_LAST_CHANCE) {
                fd = open(LOCK_FILE, O_RDONLY, 0);
                if (fd < 0) {
                    if (errno == ENOENT) {
                        LOGV("Lock file is gone now");
                        sleep_time = 0;
                    } else {
                        LOGW("Can't open lock file: %s", strerror(errno));
                    }
                } else {
                    int pid_len;

                    pid_len = read(fd, pid_buf, sizeof(pid_buf));
                    close(fd);
                    fd = -1;
                    if (pid_len > 0) {
                        int pid;
                        char* end;

                        /* Accepted format is <whitespaces><digits>\n */
                        pid = (int) strtol(pid_buf, &end, 10);

                        if ((end == NULL) || (end <= pid_buf) || (*end != '\n')) {
                            LOGV("strtol error: Can't parse owner pid");
                            pid = 0;
                        }
                        if ((pid == getpid()) || /* Owner should not be me */
                                ((pid <= 0)) || /* Invalid pid */
                                ((pid > 0) && (kill((pid_t)pid, 0) < 0) &&
                                 (errno == ESRCH))) {
                            LOGV("Deleting old lock file, was owned by %d", pid);
                            if (unlink(LOCK_FILE) < 0) {
                                LOGE("Can't delete old lock file, was owned by %d: %s",
                                     pid, strerror(errno));
                            }
                            sleep_time = 0;
                        } else {
                            LOGV("Lock file %s owned by %d",
                                    (first_pid == pid) ? "still" : "is", pid);
                            if (first_pid != pid) {
                                /* Wait a fresh set of tries and save the current owner */
                                i = MAX_LOCK_TRY;
                                first_pid = pid;
                            }
                            if (i == LOCK_TRY_LAST_CHANCE) {
                                LOGV("Deleting lock file; owner stuck or file stale");
                                if (unlink(LOCK_FILE) < 0) {
                                    LOGE("Can't delete old lock file: %s", strerror(errno));
                                }
                                sleep_time = 0;
                            }
                        }
                    } else {
                        LOGE("Can't read pid in lock file, deleting it! Len = %d. %s",
                             pid_len, (pid_len < 0) ? strerror(errno) : "");
                        if (unlink(LOCK_FILE) < 0) {
                            LOGE("Can't delete existing lock file: %s",
                                 strerror(errno));
                        }
                        sleep_time = 0;
                    }
                }
            }
        }
        if (sleep_time) {
            LOGV("Sleeping.");
            usleep(sleep_time);
        }
        i--;
    }

    if (i > 0) {
        snprintf(pid_buf, sizeof(pid_buf), "%10d\n", getpid());
        LOGV("Writing pid_buf: %s", (char *)pid_buf);
        if (write(fd, pid_buf, sizeof(pid_buf)-1) < 0) {
            LOGE("Can't write to lock file: %s", strerror(errno));
            close(fd);
            fd = -1;
        } else {
            LOGV("Lock obtained");
        }
    } else {
        LOGE("Can't obtain lock: %s", strerror(errno));
    }

    return fd;
}

static void unlock(int fd)
{
    if (fd >= 0) {
        close(fd);
        if (unlink(LOCK_FILE) < 0) {
            LOGE("Unlock unlink error: %s", strerror(errno));
        }
        else LOGI("Lock released");
    }
}

static int insmod(const char *filename, const char *args)
{
    void *module;
    unsigned int size;
    int ret;

    module = load_file(filename, &size);
    if (!module)
        return -1;

    ret = init_module(module, size, args);

    if ((ret < 0) && (errno == EEXIST)) {
        LOGV("init_module: %s is already loaded", filename);
        ret = 0;
    }

    free(module);

    return ret;
}

static int rmmod(const char *modname)
{
    int ret = -1;
    int maxtry = 10;

    while (maxtry-- > 0) {
        ret = delete_module(modname, O_NONBLOCK | O_EXCL);
        if ((ret < 0) && (errno == EAGAIN || errno == EBUSY)) {
            usleep(500000);
        }
        else if ((ret < 0) && (errno == ENOENT)) {
            LOGV("delete_module: %s is already unloaded", modname);
            ret = 0;
            break;
        }
        else
            break;
    }

    if (ret != 0)
        LOGD("Unable to unload driver module \"%s\": %s\n",
             modname, strerror(errno));
    return ret;
}

int do_dhcp_request(int *ipaddr, int *gateway, int *mask,
                    int *dns1, int *dns2, int *server, int *lease) {
    /* For test driver, always report success */
    if (strcmp(iface, WIFI_TEST_INTERFACE) == 0)
        return 0;

    if (ifc_init() < 0)
        return -1;

    if (do_dhcp(iface) < 0) {
        ifc_close();
        return -1;
    }
    ifc_close();
    get_dhcp_info(ipaddr, gateway, mask, dns1, dns2, server, lease);
    return 0;
}

const char *get_dhcp_error_string() {
    return dhcp_lasterror();
}

static int check_hotspot_driver_loaded() {
    char driver_status[PROPERTY_VALUE_MAX];
    FILE *proc;
    char line[sizeof(AP_DRIVER_MODULE_TAG)+10];
    if (!property_get(AP_DRIVER_PROP_NAME, driver_status, NULL)
            || strcmp(driver_status, "ok") != 0) {
        return 0;  /* driver not loaded */
    }
    /*
     * If the property says the driver is loaded, check to
     * make sure that the property setting isn't just left
     * over from a previous manual shutdown or a runtime
     * crash.
     */
    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        LOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
        property_set(AP_DRIVER_PROP_NAME, "unloaded");
        return 0;
    }
    while ((fgets(line, sizeof(line), proc)) != NULL) {
        if (strncmp(line, AP_DRIVER_MODULE_TAG, strlen(AP_DRIVER_MODULE_TAG)) == 0) {
            fclose(proc);
            return 1;
        }
    }
    fclose(proc);
    property_set(AP_DRIVER_PROP_NAME, "unloaded");
    return 0;
}
int hotspot_load_driver()
{
    char driver_status[PROPERTY_VALUE_MAX];
    int count = 100; /* wait at most 20 seconds for completion */
    if (check_hotspot_driver_loaded()) {
        return 0;
    }

    if (!strcmp(PRELOADER,"") == 0) {
        LOGW("Starting WIFI pre-loader");
        property_set("ctl.start", PRELOADER);
    }

#ifdef WIFI_EXT_MODULE_PATH
    if (insmod(EXT_MODULE_PATH, EXT_MODULE_ARG) < 0)
        return -1;
    usleep(200000);
#endif
    if (insmod(AP_DRIVER_MODULE_PATH, AP_DRIVER_MODULE_ARG) < 0)
        return -1;

    if (strcmp(AP_FIRMWARE_LOADER,"") == 0) {
        usleep(WIFI_DRIVER_LOADER_DELAY);
        property_set(AP_DRIVER_PROP_NAME, "ok");
    }
    else {
        property_set("ctl.start", AP_FIRMWARE_LOADER);
        usleep(WIFI_DRIVER_LOADER_DELAY);
    }
    sched_yield();
    while (count-- > 0) {
        if (property_get(AP_DRIVER_PROP_NAME, driver_status, NULL)) {
            if (strcmp(driver_status, "ok") == 0)
                return 0;
            else if (strcmp(AP_DRIVER_PROP_NAME, "failed") == 0) {
                hotspot_unload_driver();
                return -1;
            }
        }
        usleep(200000);
    }
    property_set(AP_DRIVER_PROP_NAME, "timeout");
    hotspot_unload_driver();
    return -1;
}
int hotspot_unload_driver()
{
    int count = 20; /* wait at most 10 seconds for completion */
    if (rmmod(AP_DRIVER_MODULE_NAME) == 0) {
        while (count-- > 0) {
            if (!check_hotspot_driver_loaded())
                break;
            usleep(500000);
        }
        if (count) {
#ifdef WIFI_EXT_MODULE_NAME
            if (rmmod(EXT_MODULE_NAME) == 0)
#endif
                if (!strcmp(PRELOADER,"") == 0) {
                    LOGW("Stopping WIFI pre-loader");
                    property_set("ctl.stop", PRELOADER);
                }
            return 0;
        }
        return -1;
    } else
        return -1;
}
static int check_driver_loaded() {
    char driver_status[PROPERTY_VALUE_MAX];
    FILE *proc;
    char line[sizeof(DRIVER_MODULE_TAG)+10];

    if (!property_get(DRIVER_PROP_NAME, driver_status, NULL)
            || strcmp(driver_status, "ok") != 0) {
        return 0;  /* driver not loaded */
    }
    /*
     * If the property says the driver is loaded, check to
     * make sure that the property setting isn't just left
     * over from a previous manual shutdown or a runtime
     * crash.
     */
    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        LOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
        property_set(DRIVER_PROP_NAME, "unloaded");
        return 0;
    }
    while ((fgets(line, sizeof(line), proc)) != NULL) {
        if (strncmp(line, DRIVER_MODULE_TAG, strlen(DRIVER_MODULE_TAG)) == 0) {
            fclose(proc);
            return 1;
        }
    }
    fclose(proc);
    property_set(DRIVER_PROP_NAME, "unloaded");
    return 0;
}

int wifi_load_driver()
{
    char driver_status[PROPERTY_VALUE_MAX];
    int count = 100; /* wait at most 20 seconds for completion */
    int status = -1;
    int lock_id;

    if ((lock_id = lock()) < 0)
        return -1;

    if (check_driver_loaded()) {
        unlock(lock_id);
        return 0;
    }

    property_set(DRIVER_PROP_NAME, "loading");

    if (!strcmp(PRELOADER,"") == 0) {
        LOGW("Starting WIFI pre-loader");
        property_set("ctl.start", PRELOADER);
    }

#ifdef WIFI_EXT_MODULE_PATH
    if (insmod(EXT_MODULE_PATH, EXT_MODULE_ARG) < 0)
        return -1;
    usleep(200000);
#endif

    if(system(SDIO_POLLING_ON))
        LOGW("Couldn't turn on SDIO polling: %s", SDIO_POLLING_ON);

    if ('\0' != *DRIVER_SDIO_IF_MODULE_PATH) {
       if (insmod(DRIVER_SDIO_IF_MODULE_PATH, DRIVER_SDIO_IF_MODULE_ARG) < 0) {
           goto end;
       }
    }

    if (insmod(DRIVER_MODULE_PATH, DRIVER_MODULE_ARG) < 0) {
        if ('\0' != *DRIVER_SDIO_IF_MODULE_NAME) {
           rmmod(DRIVER_SDIO_IF_MODULE_NAME);
        }
        goto end;
    }

    if (strcmp(FIRMWARE_LOADER,"") == 0) {
        usleep(WIFI_DRIVER_LOADER_DELAY);
        property_set(DRIVER_PROP_NAME, "ok");
    }
    else {
        property_set("ctl.start", FIRMWARE_LOADER);
    }
    sched_yield();
    while (count-- > 0) {
        if (property_get(DRIVER_PROP_NAME, driver_status, NULL)) {
            if (strcmp(driver_status, "ok") == 0) {
                status = 0;
                goto end;
            }
            else if (strcmp(driver_status, "failed") == 0) {
                _wifi_unload_driver();
                goto end;
            }
        }
        usleep(200000);
    }
    property_set(DRIVER_PROP_NAME, "timeout");
    _wifi_unload_driver();

end:
    system(SDIO_POLLING_OFF);
    unlock(lock_id);
    return status;
}

int wifi_unload_driver()
{
    char bt_status[PROPERTY_VALUE_MAX];
    int lock_id;
    int status;

    if (property_get("ro.config.bt.amp", bt_status, NULL)
            && (strcmp(bt_status, "yes") == 0)) {

        if (property_get("init.svc.bluetoothd", bt_status, NULL)
                && (strcmp(bt_status, "running") == 0)) {
            LOGV("Bluetooth is on; keep WiFi driver loaded for AMP PAL");
            return(0);
        }
    }

    /* Ignores possible lock failure, try to unload anyway */
    lock_id = lock();
    status = _wifi_unload_driver();
    unlock(lock_id);

    return status;
}

static int _wifi_unload_driver()
{
    int count = 20; /* wait at most 10 seconds for completion */
    char driver_status[PROPERTY_VALUE_MAX];
    int s, ret;
    struct iwreq wrq;

    /*
     * If the driver is loaded, ask it to broadcast a netlink message
     * that it will be closing, so listeners can close their sockets.
     */

    if (property_get(DRIVER_PROP_NAME, driver_status, NULL)) {
        if (strcmp(driver_status, "ok") == 0) {

            /* Equivalent to: iwpriv wlan0 sendModuleInd */
            if ((s = socket(PF_INET, SOCK_DGRAM, 0)) >= 0) {
                strncpy(wrq.ifr_name, "wlan0", IFNAMSIZ);
                wrq.u.data.length = 0; /* No Set arguments */
                wrq.u.mode = 5; /* WE_MODULE_DOWN_IND sub-command */
                ret = ioctl(s, (SIOCIWFIRSTPRIV + 1), &wrq);
                close(s);
                if (ret < 0 ) {
                    LOGE("ioctl failed: %s", strerror(errno));
                }
                sched_yield();
            }
            else {
                LOGE("Socket open failed: %s", strerror(errno));
            }
        }
    }

    if (rmmod(DRIVER_MODULE_NAME) == 0) {
        while (count-- > 0) {
            if (!check_driver_loaded())
                break;
            usleep(500000);
        }
        if (count) {

            if (rmmod(DRIVER_SDIO_IF_MODULE_NAME) == 0) {
                return 0;
            }

#ifdef WIFI_EXT_MODULE_NAME
            if (rmmod(EXT_MODULE_NAME) == 0)
#endif
                if (!strcmp(PRELOADER,"") == 0) {
                    LOGW("Stopping WIFI pre-loader");
                    property_set("ctl.stop", PRELOADER);
                }
            return 0;
        }

        return -1;
    }
    else
        return -1;
}

int ensure_config_file_exists()
{
    char buf[2048];
    int srcfd, destfd;
    int nread;
    struct stat st;

    if (access(SUPP_CONFIG_FILE, R_OK|W_OK) == 0) {
        if( stat( SUPP_CONFIG_FILE, &st ) < 0 ) {
          LOGE("Cannot stat the file \"%s\": %s", SUPP_CONFIG_FILE, strerror(errno));
          return -1;
        }
        //check if config file has some data or is it empty due to previous errors
        if( st.st_size )
          return 0;
        //else continue to write the config from default template.
    } else if (errno != ENOENT) {
        LOGE("Cannot access \"%s\": %s", SUPP_CONFIG_FILE, strerror(errno));
        return -1;
    }

    srcfd = open(SUPP_CONFIG_TEMPLATE, O_RDONLY);
    if (srcfd < 0) {
        LOGE("Cannot open \"%s\": %s", SUPP_CONFIG_TEMPLATE, strerror(errno));
        return -1;
    }

    destfd = open(SUPP_CONFIG_FILE, O_CREAT|O_WRONLY, 0660);
    if (destfd < 0) {
        close(srcfd);
        LOGE("Cannot create \"%s\": %s", SUPP_CONFIG_FILE, strerror(errno));
        return -1;
    }

    while ((nread = read(srcfd, buf, sizeof(buf))) != 0) {
        if (nread < 0) {
            LOGE("Error reading \"%s\": %s", SUPP_CONFIG_TEMPLATE, strerror(errno));
            close(srcfd);
            close(destfd);
            unlink(SUPP_CONFIG_FILE);
            return -1;
        }
        write(destfd, buf, nread);
    }

    close(destfd);
    close(srcfd);

    if (chown(SUPP_CONFIG_FILE, AID_SYSTEM, AID_WIFI) < 0) {
        LOGE("Error changing group ownership of %s to %d: %s",
             SUPP_CONFIG_FILE, AID_WIFI, strerror(errno));
        unlink(SUPP_CONFIG_FILE);
        return -1;
    }
    return 0;
}

int wifi_start_supplicant()
{
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};
    int count = 200; /* wait at most 20 seconds for completion */
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
    const prop_info *pi;
    unsigned serial = 0;
#endif

    /* Check whether already running */
    if (property_get(SUPP_PROP_NAME, supp_status, NULL)
            && strcmp(supp_status, "running") == 0) {
        return 0;
    }

    /* Before starting the daemon, make sure its config file exists */
    if (ensure_config_file_exists() < 0) {
        LOGE("Wi-Fi will not be enabled");
        return -1;
    }

    /* Clear out any stale socket files that might be left over. */
    wpa_ctrl_cleanup();

#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
    /*
     * Get a reference to the status property, so we can distinguish
     * the case where it goes stopped => running => stopped (i.e.,
     * it start up, but fails right away) from the case in which
     * it starts in the stopped state and never manages to start
     * running at all.
     */
    pi = __system_property_find(SUPP_PROP_NAME);
    if (pi != NULL) {
        serial = pi->serial;
    }
#endif
    property_set("ctl.start", SUPPLICANT_NAME);
    sched_yield();

    while (count-- > 0) {
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
        if (pi == NULL) {
            pi = __system_property_find(SUPP_PROP_NAME);
        }
        if (pi != NULL) {
            __system_property_read(pi, NULL, supp_status);
            if (strcmp(supp_status, "running") == 0) {
                return 0;
            } else if (pi->serial != serial &&
                    strcmp(supp_status, "stopped") == 0) {
                return -1;
            }
        }
#else
        if (property_get(SUPP_PROP_NAME, supp_status, NULL)) {
            if (strcmp(supp_status, "running") == 0)
                return 0;
        }
#endif
        usleep(100000);
    }
    return -1;
}

int wifi_stop_supplicant()
{
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};
    int count = 50; /* wait at most 5 seconds for completion */

    /* Check whether supplicant already stopped */
    if (property_get(SUPP_PROP_NAME, supp_status, NULL)
        && strcmp(supp_status, "stopped") == 0) {
        return 0;
    }

    property_set("ctl.stop", SUPPLICANT_NAME);
    sched_yield();

    while (count-- > 0) {
        if (property_get(SUPP_PROP_NAME, supp_status, NULL)) {
            if (strcmp(supp_status, "stopped") == 0)
                return 0;
        }
        usleep(100000);
    }
    return -1;
}

int wifi_connect_to_supplicant()
{
    char ifname[256];
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};

    /* Make sure supplicant is running */
    if (!property_get(SUPP_PROP_NAME, supp_status, NULL)
            || strcmp(supp_status, "running") != 0) {
        LOGE("Supplicant not running, cannot connect");
        return -1;
    }

    property_get("wifi.interface", iface, WIFI_TEST_INTERFACE);

    if (access(IFACE_DIR, F_OK) == 0) {
        snprintf(ifname, sizeof(ifname), "%s/%s", IFACE_DIR, iface);
    } else {
        strlcpy(ifname, iface, sizeof(ifname));
    }

    ctrl_conn = wpa_ctrl_open(ifname);
    if (ctrl_conn == NULL) {
        LOGE("Unable to open connection to supplicant on \"%s\": %s",
             ifname, strerror(errno));
        return -1;
    }
    monitor_conn = wpa_ctrl_open(ifname);
    if (monitor_conn == NULL) {
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = NULL;
        return -1;
    }
    if (wpa_ctrl_attach(monitor_conn) != 0) {
        wpa_ctrl_close(monitor_conn);
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = monitor_conn = NULL;
        return -1;
    }
    return 0;
}

int wifi_send_command(struct wpa_ctrl *ctrl, const char *cmd, char *reply, size_t *reply_len)
{
    int ret;

    if (ctrl_conn == NULL) {
        LOGV("Not connected to wpa_supplicant - \"%s\" command dropped.\n", cmd);
        return -1;
    }
    ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), reply, reply_len, NULL);
    if (ret == -2) {
        LOGD("'%s' command timed out.\n", cmd);
        return -2;
    } else if (ret < 0 || strncmp(reply, "FAIL", 4) == 0) {
        return -1;
    }
    if (strncmp(cmd, "PING", 4) == 0) {
        reply[*reply_len] = '\0';
    }
    return 0;
}

int wifi_wait_for_event(char *buf, size_t buflen)
{
    size_t nread = buflen - 1;
    int fd;
    fd_set rfds;
    int result;
    struct timeval tval;
    struct timeval *tptr;
    
    if (monitor_conn == NULL) {
        LOGD("Connection closed\n");
        strncpy(buf, WPA_EVENT_TERMINATING " - connection closed", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }

    result = wpa_ctrl_recv(monitor_conn, buf, &nread);
    if (result < 0) {
        LOGD("wpa_ctrl_recv failed: %s\n", strerror(errno));
        strncpy(buf, WPA_EVENT_TERMINATING " - recv error", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }
    buf[nread] = '\0';
    /* LOGD("wait_for_event: result=%d nread=%d string=\"%s\"\n", result, nread, buf); */
    /* Check for EOF on the socket */
    if (result == 0 && nread == 0) {
        /* Fabricate an event to pass up */
        LOGD("Received EOF on supplicant socket\n");
        strncpy(buf, WPA_EVENT_TERMINATING " - signal 0 received", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }
    /*
     * Events strings are in the format
     *
     *     <N>CTRL-EVENT-XXX 
     *
     * where N is the message level in numerical form (0=VERBOSE, 1=DEBUG,
     * etc.) and XXX is the event name. The level information is not useful
     * to us, so strip it off.
     */
    if (buf[0] == '<') {
        char *match = strchr(buf, '>');
        if (match != NULL) {
            nread -= (match+1-buf);
            memmove(buf, match+1, nread+1);
        }
    }
    return nread;
}

void wifi_close_supplicant_connection()
{
    if (ctrl_conn != NULL) {
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = NULL;
    }
    if (monitor_conn != NULL) {
        wpa_ctrl_close(monitor_conn);
        monitor_conn = NULL;
    }
}

int wifi_command(const char *command, char *reply, size_t *reply_len)
{
    return wifi_send_command(ctrl_conn, command, reply, reply_len);
}

