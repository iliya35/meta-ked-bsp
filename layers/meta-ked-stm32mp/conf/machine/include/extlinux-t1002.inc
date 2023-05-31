# =========================================================================
# Extlinux boot commands for
# Board: stm32mp-t1002
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
	stm32mp-t1002_sdcard \
	stm32mp-t1002_nor-nand \
"

# definitions for this target *_sdcard boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1002_sdcard    = "mmc0_stm32mp-t1002_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1002_sdcard          = "stm32mp-t1002-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1002_sdcard   = "stm32mp-t1002-sdcard"
UBOOT_EXTLINUX_FDT_stm32mp-t1002-sdcard             = "/boot/stm32mp-t1002.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1002-sdcard            = "root=/dev/mmcblk0p4"

# definitions for this target *_nor-nand boot
UBOOT_EXTLINUX_BOOTPREFIXES_stm32mp-t1002_nor-nand  = "ubi0_stm32mp-t1002_"
UBOOT_EXTLINUX_LABELS_stm32mp-t1002_nor-nand        = "stm32mp-t1002-nor-nand"
UBOOT_EXTLINUX_DEFAULT_LABEL_stm32mp-t1002_nor-nand = "stm32mp-t1002-nor-nand"
UBOOT_EXTLINUX_FDT_stm32mp-t1002-nor-nand           = "/boot/stm32mp-t1002.dtb"
UBOOT_EXTLINUX_ROOT_stm32mp-t1002-nor-nand          = "ubi.mtd=5 rootfstype=ubifs root=ubi0_0"
