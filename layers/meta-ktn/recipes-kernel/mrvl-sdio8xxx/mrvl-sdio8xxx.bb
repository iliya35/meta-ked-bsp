SUMMARY = "Marvell/NXP sdio8977 Kernel Module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=aabffeed19e84f5c15020056b4c36d59"

# Driver Release 16679 (Version C4X16679_V2)

RDEPENDS:${PN} = "linux-firmware-sd8977"
RPROVIDES:${PN} += "kernel-module-sd8xxx"

inherit module

SRC_URI = " \
     file://Makefile \
     file://mlan \
     file://mlinux \
     file://mapp \
     file://COPYING \
"

S = "${WORKDIR}"

KERNEL_MODULE_AUTOLOAD += "sd8xxx"
KERNEL_MODULE_PROBECONF += "sd8xxx"

# Set some parameters for the driver module.
# cal_data_cfg=none is necessary to use the calibration data stored on the module.
# See PAN9026 Software Guide and driver manual for more information.
module_conf_sd8xxx = "options sd8xxx drv_mode=7 fw_name=mrvl/sdsd8977_combo_v2.bin cal_data_cfg=none cfg80211_wext=0xf cfg80211_drcs=0"

