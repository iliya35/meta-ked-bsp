require conf/machine/include/tune-cortexa7.inc

MACHINEOVERRIDES =. "mx6:mx6ul:ktn-mx6ul:"

UBOOT_MAKE_TARGET ?= "flash.bin"
UBOOT_BINARY ?= "flash.bin"
UBOOT_SUFFIX ?= "bin"

UBOOT_DTB_LOADADDRESS ?= ""
UBOOT_LOADADDRESS ?= "0x81000000"

UBOOT_EXTLINUX_CONSOLE = "console=ttymxc3,115200"
UBOOT_EXTLINUX_ROOT = "root=/dev/mmcblk0p2"

WKS_FILE = "image-imx-ktn.wks"
IMX_BOOT_SEEK = "1"
WICVARS:append = " IMX_BOOT_SEEK UBOOT_BINARY"

UBOOT_ENTRYPOINT:mx6ul:remove = "0x10008000"
UBOOT_ENTRYPOINT:ktn-mx6ul:append = "${UBOOT_LOADADDRESS}"
