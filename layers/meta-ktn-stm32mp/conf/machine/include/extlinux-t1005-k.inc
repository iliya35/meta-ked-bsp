# =========================================================================
# Extlinux boot commands for
# Board: stm32mp-t1005-k
# =========================================================================
# Define boot command lines
#
# The following bootschemes are supported
# - boot from sdcard (BOOTPIN_0,2=1, BOOTPIN_1=0)
# - boot from nor (BOOTPIN_0=1, BOOTPIN_3,2=0)
#   When booting from nor u-boot variable 'boot_targets' select where to find
#   the bootfs with EXTLINUX configuration
#
# This is a multi configuration file for all t1005-k configurations. With
# extlinux it is possible to choose your different machine even when you have the
# default t1005-k u-boot configuration

# add targets to generate
UBOOT_EXTLINUX_TARGETS += "\
	stm32mp-t1005-k_sdcard \
	stm32mp-t1005-k_nor-emmc \
	stm32mp-t1005-k_nor-nand \
	"

# definitions for this target *_sdcard boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1005-k_sdcard    = "mmc0_stm32mp-t1005-k_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1005-k_sdcard          = "stm32mp-t1005-k-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1005-k_sdcard   ?= "stm32mp-t1005-k-sdcard"
# label t1005-k
UBOOT_EXTLINUX_FDT_stm32mp-t1005-k-sdcard             = "/boot/stm32mp-t1005-k.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1005-k-sdcard            = "root=/dev/mmcblk0p4"

# definitions for this target *_nor-emmc boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1005-k_nor-emmc  = "mmc1_stm32mp-t1005-k_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1005-k_nor-emmc        = "stm32mp-t1005-k-nor-emmc"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1005-k_nor-emmc ?= "stm32mp-t1005-k-nor-emmc"
# label t1005-k
UBOOT_EXTLINUX_FDT_stm32mp-t1005-k-nor-emmc           = "/boot/stm32mp-t1005-k.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1005-k-nor-emmc          = "root=/dev/mmcblk1p1"

# definitions for this target *_nor-nand boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1005-k_nor-nand  = "ubi0_stm32mp-t1005-k_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1005-k_nor-nand        = "stm32mp-t1005-k-nor-nand"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1005-k_nor-nand ?= "stm32mp-t1005-k-nor-nand"
# label t1005-k
UBOOT_EXTLINUX_FDT_stm32mp-t1005-k-nor-nand           = "/stm32mp-t1005-k.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1005-k-nor-nand          = "ubi.mtd=5 rootfstype=ubifs root=ubi0_0"
