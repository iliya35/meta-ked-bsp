#!/bin/sh

# ptool settings for Kontron i.MX6UL boards

# bootconfiguration
# bits   31-24     23-16     15-8      7-0
# [ BOOT_CFG4 BOOT_CFG3 BOOT_CFG2 BOOT_CFG1
# boot device select           : 40: SD/eSD
# BOOT_CFG2: sd config         : 20: 4-bit auf SDHC1
# BOOT_CFG3: cpu/ddr config    : 00: single DDR, 792/396 MHz CPU/DDR
# BOOT_CFG4: alternativ bootdev: 49: enable, eCSPIx_SS0 chip select, 3-Byte eCSPI2 (NOR Flash)
BOOT_CONFIG=0x49002040

SPL_BIN=flash.bin
UBOOT_ENV=u-boot-initial-env-kontron-mx6ul
ROOTFS_IMG=image-ktn-kontron-mx6ul.tar.gz
BOOTFS_IMG=image-ktn-bootfs-kontron-mx6ul-ktn-dunfell.tar.gz	
USERFS_IMG=image-ktn-userfs-kontron-mx6ul-ktn-dunfell.tar.gz
SWU_ARCHIVE="swupdate-img.swu"

MTD_ROOT=/dev/mtd0
MTD_UBOOT_SPL=/dev/mtd1
MTD_UBOOT_ENV=/dev/mtd2
MTD_UBOOT_ENV_REDUNDANT=/dev/mtd3

#partition size on NAND-Flash (512MB)
rootfs_size=150 
bootfs_size=30
#userfs (if size empty -> all remaining space taken else SIZE in MiB)
userfs_size="" 

FW_DIR="/home/root/fw"		# Use persistent dir on SD card to cache FW
SRC_MEDIA="HTTP_SERVER"
HTTP_URL="https://files.kontron-electronics.de/imx/latest/kontron-mx6ul"

EMMC_DEV=/dev/mmcblk1
EMMC_PART=/dev/mmcblk1p1

b_flash_emmc=0
b_flash_ubi=1
b_lan2_mac_otp=1
