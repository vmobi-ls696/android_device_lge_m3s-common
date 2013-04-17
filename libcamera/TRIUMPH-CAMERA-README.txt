
This is to keep track of what I know about the camera. It's certainly
not guaranteed to be right.

The camera is operated by four separate modules, now two of them are
binary, and quite frankly it's pretty scary this scheme actually
works. The MD5s of the binaries I am using:

f72f6ffb2df03f48e30984e61ba98eeb liboemcamera.so
5a5eac1a832129bfd7a37d60f547a7ee libcamera.so

First up is the kernel driver, in drivers/media/video/msm. This seems
to be mostly i2c control functions, although the version of it is
critical to the camera working for reasons I don't yet know, possibly
a difference in ioctl message values. The kernel driver I pulled is
from:

triumph-kernel-msm7x30-46cea7ad007cf38438208ce14dd1c3b4890f70c3

Second, is QualcommCameraHardware.cpp, aka libcamera.so. It doesn't
look like we actually have this source code file from a Triumph. I did
play around with a couple ideas with QualcommCameraHardware.cpp, but
could never get things working. So, I decided to use the binary
libcamera.so that we know is working from CM7. One issue with using
the binary is that the content of QCamera_Intf.h needs to match the
binary liboemcamera.so, especially cam_ctrl_type, as there is a least
one jump table in the binary that depends on its values. I have
changed QCamera_Intf.h to match libeomcamera.so as best I could.

Third, is the cameraHAL, new to ICS, which is camera.msm7x30.so. This
contains a copy of QualcommCameraHardware.cpp, so I changed the build
to link directly with the binary libcamera.so. It is worth noting that
this library is actually loaded by the mediaserver process, and that
if you want to replace it on a live phone, you need to kill the
mediaserver process before the changes will take effect. There
is an msm_camera.h here that should probably be kept in sync with the
kernel's. Also, The CameraHardwareInterface class contains a vtable
that needs to be kept in sync with libcamera.so.

Last up is the camera framework. This is part of the C++ code that is made
into JNI so it can be called from Java. There is an important piece of
the puzzle here as well: CameraParameters.cpp/h. Certainly
libcamera.so depends on the parameters being correct and possibly
liboemcamera.so as well. I pulled these from cm7 frameworks.

Note that libcamera2.so is not used.
