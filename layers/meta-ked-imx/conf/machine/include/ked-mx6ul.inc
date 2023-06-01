include conf/machine/include/tune-cortexa7.inc
include conf/machine/include/arm/armv7a/tune-cortexa7.inc

MACHINEOVERRIDES =. "ked-mx6ul:ked-mx6:ked-imx:mx6ul:mx6:"

UBOOT_MAKE_TARGET ?= "flash.bin"
UBOOT_BINARY ?= "flash.bin"
UBOOT_SUFFIX ?= "bin"

UBOOT_DTB_LOADADDRESS ?= ""
UBOOT_LOADADDRESS ?= "0x81000000"

UBOOT_EXTLINUX_CONSOLE = "console=ttymxc3,115200"
UBOOT_EXTLINUX_ROOT = "root=/dev/mmcblk0p2"

WKS_FILE = "image-imx-ked.wks"
IMX_BOOT_SEEK = "1"
WICVARS:append = " IMX_BOOT_SEEK UBOOT_BINARY"

UBOOT_ENTRYPOINT = "${UBOOT_LOADADDRESS}"
UBOOT_ENTRYPOINT:mx6ul-generic-bsp  = "${UBOOT_LOADADDRESS}"
UBOOT_ENTRYPOINT:mx6ull-generic-bsp = "${UBOOT_LOADADDRESS}"
