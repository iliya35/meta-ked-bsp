SUMMARY = "Marvell/NXP bt8977 Kernel Module"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://README;md5=aad5a77b539d79988e91858388e85521"

# Driver Release 16679 (Version C4X16679_V2)

RDEPENDS:${PN} = "linux-firmware-sd8977"
RPROVIDES:${PN} += "kernel-module-bt8xxx"

inherit module

SRC_URI = " \
     file://Makefile	\
     file://app		\
     file://bt		\
     file://README	\
"

S = "${WORKDIR}"

KERNEL_MODULE_AUTOLOAD += "bt8xxx"
KERNEL_MODULE_PROBECONF += "bt8xxx"

module_conf_bt8xxx = "options bt8xxx cal_cfg=mrvl/bt_cal_data.conf init_cfg=mrvl/bt_init_cfg.conf"
