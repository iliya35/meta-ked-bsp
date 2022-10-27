# =========================================================================
# Extlinux boot commands for
# Board: stm32mp-t1000-k-101
# =========================================================================
# Define boot command lines
#
# The following bootschemes are supported
# - boot from sdcard (BOOTPIN_0,2=1, BOOTPIN_1=0)
# - boot from nor (BOOTPIN_0=1, BOOTPIN_3,2=0)
#   When booting from nor u-boot variable 'boot_targets' select where to find
#   the bootfs with EXTLINUX configuration
#

# add targets to generate
UBOOT_EXTLINUX_TARGETS += "\
	stm32mp-t1000-k-101_sdcard \
	stm32mp-t1000-k-101_nor-emmc \
	stm32mp-t1000-k-101_nor-nand \
"

# definitions for this target *_sdcard boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1000-k-101_sdcard    = "mmc0_stm32mp-t1000-k-101_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1000-k-101_sdcard          = "stm32mp-t1000-k-101-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1000-k-101_sdcard   = "stm32mp-t1000-k-101-sdcard"
# label t1000-k-101
UBOOT_EXTLINUX_FDT_stm32mp-t1000-k-101-sdcard             = "/boot/stm32mp-t1000-k-101.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1000-k-101-sdcard            = "root=/dev/mmcblk0p4"

# definitions for this target *_nor-emmc boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1000-k-101_nor-emmc  = "mmc1_stm32mp-t1000-k-101_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1000-k-101_nor-emmc        = "stm32mp-t1000-k-101-nor-emmc"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1000-k-101_nor-emmc = "stm32mp-t1000-k-101-nor-emmc"
# label t1000-k-101
UBOOT_EXTLINUX_FDT_stm32mp-t1000-k-101-nor-emmc           = "/boot/stm32mp-t1000-k-101.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1000-k-101-nor-emmc          = "root=/dev/mmcblk1p1"

# definitions for this target *_nor-nand boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1000-k-101_nor-nand  = "ubi0_stm32mp-t1000-k-101_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1000-k-101_nor-nand        = "stm32mp-t1000-k-101-nor-nand"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1000-k-101_nor-nand = "stm32mp-t1000-k-101-nor-nand"
# label t1000-k-101
UBOOT_EXTLINUX_FDT_stm32mp-t1000-k-101-nor-nand           = "/boot/stm32mp-t1000-k-101.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1000-k-101-nor-nand          = "ubi.mtd=5 rootfstype=ubifs root=ubi0_0"
