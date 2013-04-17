$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

$(call inherit-product, frameworks/base/build/phone-hdpi-512-dalvik-heap.mk)

$(call inherit-product, vendor/cm/config/common_full_phone.mk)

$(call inherit-product, vendor/cm/config/cdma.mk)

$(call inherit-product, vendor/cm/config/themes_common.mk)

$(call inherit-product, build/target/product/full.mk)

DEVICE_PACKAGE_OVERLAYS += device/lge/m3s-common/overlay

PRODUCT_AAPT_CONFIG := normal mdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi

PRODUCT_PROPERTY_OVERRIDES += \
	ro.sf.lcd_density=160

TARGET_BOOTANIMATION_NAME := vertical-320x480

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/base/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/base/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/base/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/base/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
    frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/base/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/base/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/base/nfc-extras/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml \
    device/lge/m3s-common/configs/qcnvitems.xml:system/etc/permissions/qcnvitems.xml \
    device/lge/m3s-common/configs/qcrilhook.xml:system/etc/permissions/qcrilhook.xml \
    device/lge/m3s-common/configs/qcom_oncrpc.xml:system/etc/permissions/qcom_oncrpc.xml \
  packages/apps/Nfc/migrate_nfc.txt:system/etc/updatecmds/migrate_nfc.txt \
  device/sample/nxp/com.nxp.mifare.xml:system/etc/permissions/com.nxp.mifare.xml
#    device/lge/m3s-common/configs/lgsvcitems.xml:system/etc/permissions/lgsvcitems.xml 

PRODUCT_COPY_FILES += \
device/lge/m3s-common/configs/qcrilhook.jar:system/framework/qcrilhook.jar \
device/lge/m3s-common/configs/qcnvitems.jar:system/framework/qcnvitems.jar 
#device/lge/m3s-common/configs/lgflag.jar:system/framework/lgflag.jar \
#device/lge/m3s-common/configs/lgsvcitems.jar:system/framework/lgsvcitems.jar

PRODUCT_COPY_FILES += \
device/lge/m3s-common/prebuilt/lib/libC2D2.so:system/lib/libC2D2.so \
device/lge/m3s-common/prebuilt/lib/libgsl.so:system/lib/libgsl.so \
device/lge/m3s-common/prebuilt/lib/libOpenVG.so:system/lib/libOpenVG.so \
device/lge/m3s-common/prebuilt/lib/libsc-a2xx.so:system/lib/libsc-a2xx.so \
device/lge/m3s-common/prebuilt/lib/egl/egl.cfg:system/lib/egl/egl.cfg \
device/lge/m3s-common/prebuilt/lib/egl/eglsubAndroid.so:system/lib/egl/eglsubAndroid.so \
device/lge/m3s-common/prebuilt/lib/egl/libEGL_adreno200.so:system/lib/egl/libEGL_adreno200.so \
device/lge/m3s-common/prebuilt/lib/egl/libGLES_android.so:system/lib/egl/libGLES_android.so \
device/lge/m3s-common/prebuilt/lib/egl/libGLESv1_CM_adreno200.so:system/lib/egl/libGLESv1_CM_adreno200.so \
device/lge/m3s-common/prebuilt/lib/egl/libGLESv2_adreno200.so:system/lib/egl/libGLESv2_adreno200.so \
device/lge/m3s-common/prebuilt/lib/egl/libq3dtools_adreno200.so:system/lib/egl/libq3dtools_adreno200.so 

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/prebuilt/vold.fstab:system/etc/vold.fstab

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/keychars/atcmd_virtual_kbd.kcm.bin:system/usr/keychars/atcmd_virtual_kbd.kcm.bin \
    device/lge/m3s-common/keychars/ats_input.kcm.bin:system/usr/keychars/ats_input.kcm.bin \
    device/lge/m3s-common/keychars/lge_m3s_keypad.kcm.bin:system/usr/keychars/lge_m3s_keypad.kcm.bin \
    device/lge/m3s-common/keychars/qwerty.kcm.bin:system/usr/keychars/qwerty.kcm.bin \
    device/lge/m3s-common/keychars/qwerty2.kcm.bin:system/usr/keychars/qwerty2.kcm.bin \
    device/lge/m3s-common/keylayout/7k_handset.kl:system/usr/keylayout/7k_handset.kl \
    device/lge/m3s-common/keylayout/AVRCP.kl:system/usr/keylayout/AVRCP.kl \
    device/lge/m3s-common/keylayout/atcmd_virtual_kbd.kl:system/usr/keylayout/atcmd_virtual_kbd.kl \
    device/lge/m3s-common/keylayout/ats_input.kl:system/usr/keylayout/ats_input.kl \
    device/lge/m3s-common/keylayout/gpio-side-keypad.kl:system/usr/keylayout/gpio-side-keypad.kl \
    device/lge/m3s-common/keylayout/lge_m3s_keypad.kl:system/usr/keylayout/lge_m3s_keypad.kl \
    device/lge/m3s-common/keylayout/qwerty.kl:system/usr/keylayout/qwerty.kl \
    device/lge/m3s-common/keylayout/touch_mcs8000.kl:system/usr/keylayout/touch_mcs8000.kl

PRODUCT_COPY_FILES +=	\
    device/lge/m3s-common/usr/idc/atcmd_virtual_kbd.idc:system/usr/idc/atcmd_virtual_kbd.idc \
    device/lge/m3s-common/usr/idc/gpio-side-keypad.idc:system/usr/idc/gpio-side-keypad.idc \
    device/lge/m3s-common/usr/idc/touch_mcs8000.idc:system/usr/idc/touch_mcs8000.idc \
    device/lge/m3s-common/usr/idc/gpio-side-keypad.idc:system/usr/idc/gpio-side-keypad.idc

PRODUCT_COPY_FILES +=	\
    device/lge/m3s-common/usr/idc/touch_mcs8000.idc:system/usr/idc/touch_mcs8000.idc \

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/sensors/acdapi/calib.dat:system/etc/sensors/acdapi/calib.dat \
    device/lge/m3s-common/sensors/acdapi/param.dat:system/etc/sensors/acdapi/param.dat \
    device/lge/m3s-common/sensors/acdapi/sensor_data_init.sh:system/etc/sensors/acdapi/sensor_data_init.sh \
    device/lge/m3s-common/sensors/acdapi/sensors.dat:system/etc/sensors/acdapi/sensors.dat

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/prebuilt/init.qcom.sdio.sh:system/etc/init.qcom.sdio.sh \
    device/lge/m3s-common/prebuilt/format_fat32.sh:system/etc/format_fat32.sh \
    device/lge/m3s-common/prebuilt/format_first.sh:system/etc/format_first.sh \
    device/lge/m3s-common/prebuilt/init.qcom.bt.sh:system/etc/init.qcom.bt.sh \
    device/lge/m3s-common/prebuilt/init.qcom.coex.sh:system/etc/init.qcom.coex.sh \
    device/lge/m3s-common/prebuilt/init.qcom.fm.sh:system/etc/init.qcom.fm.sh \
    device/lge/m3s-common/prebuilt/init.qcom.ftm_module.sh:system/etc/init.qcom.ftm_module.sh \
    device/lge/m3s-common/prebuilt/init.qcom.ftm_module_out.sh:system/etc/init.qcom.ftm_module_out.sh \
    device/lge/m3s-common/prebuilt/init.qcom.post_boot.sh:system/etc/init.qcom.post_boot.sh \
    device/lge/m3s-common/prebuilt/init.qcom.sdio.sh:system/etc/init.qcom.sdio.sh \
    device/lge/m3s-common/prebuilt/init.qcom.wifi.sh:system/etc/init.qcom.wifi.sh \
    device/lge/m3s-common/prebuilt/last_kmsg_backup.sh:system/etc/last_kmsg_backup.sh

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/configs/20-dns.conf:system/etc/dhcpcd/dhcpcd-hooks/20-dns.conf \
    device/lge/m3s-common/configs/95-configured:system/etc/dhcpcd/dhcpcd-hooks/95-configured \
    device/lge/m3s-common/prebuilt/dhcpcd-run-hooks:system/etc/dhcpcd/dhcpcd-run-hooks \

PRODUCT_COPY_FILES += \
device/lge/m3s-common/prebuilt/ip-up-vpn:system/etc/ppp/ip-up-vpn \

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/configs/gps.conf:system/etc/gps.conf \
    device/lge/m3s-common/configs/egl.cfg:system/lib/egl/egl.cfg \
    device/lge/m3s-common/configs/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    device/lge/m3s-common/configs/OperatorPolicy.xml:system/etc/OperatorPolicy.xml \
    device/lge/m3s-common/configs/UserPolicy.xml:system/etc/UserPolicy.xml \
    device/lge/m3s-common/configs/01_qcomm_omx.cfg:system/etc/01_qcomm_omx.cfg \
    device/lge/m3s-common/configs/loc_parameter.ini:system/etc/loc_parameter.ini \
    device/lge/m3s-common/configs/auto_pairing.conf:system/etc/auto_pairing.conf \
    device/lge/m3s-common/configs/pvplayer.cfg:system/etc/pvplayer.cfg \
    device/lge/m3s-common/configs/thermald.conf:system/etc/thermald.conf \
    device/lge/m3s-common/configs/input.conf:system/etc/input.conf \
    device/lge/m3s-common/configs/main.conf:system/etc/main.conf \
    device/lge/m3s-common/configs/dbus.conf:system/etc/dbus.conf \
    device/lge/m3s-common/configs/blacklist.conf:system/etc/blacklist.conf \
    device/lge/m3s-common/configs/audio.conf:system/etc/audio.conf 

PRODUCT_COPY_FILES +=\
    device/lge/m3s-common/root/chargerimages/battery_charging_08.rle:root/chargerimages/battery_charging_08.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_02.rle:root/chargerimages/battery_charging_02.rle \
    device/lge/m3s-common/root/chargerimages/battery_ani_05.rle:root/chargerimages/battery_ani_05.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_10.rle:root/chargerimages/battery_charging_10.rle \
    device/lge/m3s-common/root/chargerimages/battery_wait_ani_02.rle:root/chargerimages/battery_wait_ani_02.rle \
    device/lge/m3s-common/root/chargerimages/battery_notenough.rle:root/chargerimages/battery_notenough.rle \
    device/lge/m3s-common/root/chargerimages/black_bg.rle:root/chargerimages/black_bg.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_09.rle:root/chargerimages/battery_charging_09.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_12.rle:root/chargerimages/battery_charging_12.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_06.rle:root/chargerimages/battery_charging_06.rle \
    device/lge/m3s-common/root/chargerimages/battery2.rle:root/chargerimages/battery2.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_07.rle:root/chargerimages/battery_charging_07.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_11.rle:root/chargerimages/battery_charging_11.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_01.rle:root/chargerimages/battery_charging_01.rle \
    device/lge/m3s-common/root/chargerimages/battery_bg.rle:root/chargerimages/battery_bg.rle \
    device/lge/m3s-common/root/chargerimages/battery_ani_03.rle:root/chargerimages/battery_ani_03.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_04.rle:root/chargerimages/battery_charging_04.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_03.rle:root/chargerimages/battery_charging_03.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_05.rle:root/chargerimages/battery_charging_05.rle \
    device/lge/m3s-common/root/chargerimages/battery_ani_04.rle:root/chargerimages/battery_ani_04.rle \
    device/lge/m3s-common/root/chargerimages/battery_charging_00.rle:root/chargerimages/battery_charging_00.rle \
    device/lge/m3s-common/root/chargerimages/battery_ani_01.rle:root/chargerimages/battery_ani_01.rle \
    device/lge/m3s-common/root/chargerimages/battery_ani_02.rle:root/chargerimages/battery_ani_02.rle \
    device/lge/m3s-common/root/chargerimages/battery_wait_ani_01.rle:root/chargerimages/battery_wait_ani_01.rle \
    device/lge/m3s-common/root/sbin/bootlogo:root/sbin/bootlogo \
    device/lge/m3s-common/root/sbin/e2fsck_static:root/sbin/e2fsck_static \
    device/lge/m3s-common/root/sbin/tune2fs_static:root/sbin/tune2fs_static \
    device/lge/m3s-common/root/sbin/lge_fota:root/sbin/lge_fota \
    device/lge/m3s-common/root/sbin/mke2fs_static:root/sbin/mke2fs_static \
    device/lge/m3s-common/root/sbin/resize2fs_static:root/sbin/resize2fs_static \
    device/lge/m3s-common/root/sbin/chargerlogo:root/sbin/chargerlogo \
    device/lge/m3s-common/root/sbin/lge_fota:root/sbin/lge_fota \
    device/lge/m3s-common/root/sbin/adbd:root/sbin/adbd \
    device/lge/m3s-common/root/init:root/init \
    device/lge/m3s-common/root/bootimages/LG_opening_logo.rle:root/bootimages/LG_opening_logo.rle \
    device/lge/m3s-common/root/init.qcom.sh:root/init.qcom.sh \
    device/lge/m3s-common/root/ueventd.rc:root/ueventd.rc \
    device/lge/m3s-common/root/init.target.rc:root/init.target.rc \
    device/lge/m3s-common/root/init.qcom.rc:root/init.qcom.rc \
    device/lge/m3s-common/root/lgdms.fota.rc:root/lgdms.fota.rc \
    device/lge/m3s-common/root/lgdms.fota_update.rc:root/lgdms.fota_update.rc \
    device/lge/m3s-common/root/init.rc:root/init.rc 

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/firmware/a225p5_pm4.fw:system/etc/firmware/a225p5_pm4.fw \
    device/lge/m3s-common/firmware/a225_pfp.fw:system/etc/firmware/a225_pfp.fw \
    device/lge/m3s-common/firmware/a225_pm4.fw:system/etc/firmware/a225_pm4.fw \
    device/lge/m3s-common/firmware/a300_pfp.fw:system/etc/firmware/a300_pfp.fw \
    device/lge/m3s-common/firmware/a300_pm4.fw:system/etc/firmware/a300_pm4.fw \
    device/lge/m3s-common/firmware/a225p5_pm4.fw:system/etc/firmware/a225p5_pm4.fw \
    device/lge/m3s-common//firmware/cyttsp_7630_fluid.hex:system/etc/firmware/cyttsp_7630_fluid.hex \
    device/lge/m3s-common/firmware/leia_pfp_470.fw:system/etc/firmware/leia_pfp_470.fw \
    device/lge/m3s-common/firmware/leia_pm4_470.fw:system/etc/firmware/leia_pm4_470.fw \
    device/lge/m3s-common/firmware/vidc_720p_command_control.fw:system/etc/firmware/vidc_720p_command_control.fw \
    device/lge/m3s-common/firmware/vidc_720p_h263_dec_mc.fw:system/etc/firmware/vidc_720p_h263_dec_mc.fw \
    device/lge/m3s-common/firmware/vidc_720p_h264_dec_mc.fw:system/etc/firmware/vidc_720p_h264_dec_mc.fw \
    device/lge/m3s-common/firmware/vidc_720p_h264_enc_mc.fw:system/etc/firmware/vidc_720p_h264_enc_mc.fw \
    device/lge/m3s-common/firmware/vidc_720p_mp4_dec_mc.fw:system/etc/firmware/vidc_720p_mp4_dec_mc.fw \
    device/lge/m3s-common/firmware/vidc_720p_mp4_enc_mc.fw:system/etc/firmware/vidc_720p_mp4_enc_mc.fw \
    device/lge/m3s-common/firmware/vidc_720p_vc1_dec_mc.fw:system/etc/firmware/vidc_720p_vc1_dec_mc.fw \
    device/lge/m3s-common/firmware/yamato_pfp.fw:system/etc/firmware/yamato_pfp.fw	\
    device/lge/m3s-common/firmware/yamato_pm4.fw:system/etc/firmware/yamato_pm4.fw

PRODUCT_COPY_FILES += \
device/lge/m3s-common/prebuilt/wifi/WCN1314_rf.ko:system/lib/modules/volans/WCN1314_rf.ko \
device/lge/m3s-common/prebuilt/wifi/WCN1314_rf_ftm.ko:system/lib/modules/volans/WCN1314_rf_ftm.ko \
device/lge/m3s-common/prebuilt/wifi/wlan.ko:system/lib/modules/wlan.ko 

PRODUCT_COPY_FILES += \
    device/lge/m3s-common/configs/media_profiles.xml:system/etc/media_profiles.xml \
    device/lge/m3s-common/configs/media_codecs.xml:system/etc/media_codecs.xml

PRODUCT_COPY_FILES += \
device/lge/m3s-common/prebuilt/scripts/SDCard98:system/etc/init.d/SDcard98 \
device/lge/m3s-common/prebuilt/scripts/Ramscript:system/etc/init.d/Ramscript

# Audio
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio_policy.msm7x30 \
    audio.primary.msm7x30 \
    libaudioutils

# Display
PRODUCT_PACKAGES += \
    copybit.msm7x30 \
    gralloc.msm7x30 \
    hwcomposer.msm7x30 \
    libgenlock \
    libmemalloc \
    liboverlay \
    libQcomUI \
    libtilerenderer 

# Media
PRODUCT_PACKAGES += \
    mm-vdec-omx-test \
    mm-venc-omx-test720p \
    libdivxdrmdecrypt \
    libstagefrighthw \
    libOmxCore \
    libOmxVdec \
    libOmxVenc \
    OMXCore \
    libOMX_Core \
    libdomx

PRODUCT_PACKAGES += \
    libskiahwdec \
    libskiahwenc

# Sensor/Camera/Lights/GPS HAL, Misc
PRODUCT_PACKAGES += \
    libcamera \
    camera.msm7x30 \
    com.android.future.usb.accessory \
    gps.lge_m3s \
    lights.lge_m3s \
    sensors.default

# Live Wallpapers
PRODUCT_PACKAGES += \
    LiveWallpapers \
    LiveWallpapersPicker \
    MagicSmokeWallpapers \
    VisualizationWallpapers \
    librs_jni

# NFC HAL
PRODUCT_PACKAGES += \
    nfc.msm7x30

# NFC
PRODUCT_PACKAGES += \
    Nfc \
    Tag \
    libnfc_jni \
    libnfc \
    com.android.nfc_extras 

# BT
PRODUCT_PACKAGES += \
    hcitool \
    hci_qcomm_init \
    hciconfig 
      
# NFCEE Access
PRODUCT_COPY_FILES += \
device/lge/m3s-common/configs/nfcee_access_debug.xml:system/etc/nfcee_access.xml
