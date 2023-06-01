#!/bin/sh

# ptool settings for Kontron i.MX8MM boards

SOC_FAMILY="mx8mm"

#BOOT_CONFIG=0x18002020 	# Boot from eMMC (SD1, 8bit buswidth, 3.3V)
#BOOT_CONFIG2=0x03000010	# Use SPI NOR as fallback (eCSPI1, CS0)

BOOT_CONFIG=0x18001410 		# Boot from SD (SD2, 4bit buswidth, 3.3V)
BOOT_CONFIG2=0x03000010		# Use SPI NOR as fallback (eCSPI1, CS0)

#Image Files
SPL_BIN=flash.bin
UBOOT_ENV=u-boot-initial-env-kontron-mx8mm
ROOTFS_IMG=image-ked-kontron-mx8mm.tar.gz
BOOTFS_IMG=image-ked-bootfs-kontron-mx8mm-ked-dunfell.tar.gz	
USERFS_IMG=image-ked-userfs-kontron-mx8mm-ked-dunfell.tar.gz
SWU_ARCHIVE="swupdate-img.swu"

# check images with md5sum (md5sum.IMAGE_NAME.txt  file must be available)
# set to "yes" when desired
images_md5=""

#partition sizes on eMMC (MB)
rootfs_size=10000
bootfs_size=50
#userfs (if size empty -> all remaining space taken else SIZE in MiB)
userfs_size="" 

#partition devices
MTD_UBOOT_SPL=/dev/mtd0
MTD_UBOOT_ENV=/dev/mtd1
MTD_UBOOT_ENV_REDUNDANT=/dev/mtd2
EMMC_DEV=/dev/mmcblk0
EMMC_PART=/dev/mmcblk0p2

FW_DIR="/home/root/fw"		# Use persistent dir on SD card to cache FW
SRC_MEDIA="HTTP_SERVER"
HTTP_URL="https://files.kontron-electronics.de/imx/latest/kontron-mx8mm"

MD5_PRE="$FW_DIR/md5sum."
MD5_UBOOT_BIN=$MD5_PRE$UBOOT_BIN
MD5_UBOOT_ENV=$MD5_PRE$UBOOT_ENV
MD5_ROOT_UBI=$MD5_PRE$ROOT_UBI
MD5_ROOT_EXT=$MD5_PRE$ROOT_EXT


b_flash_emmc=1
b_flash_ubi=0
b_lan2_mac_otp=1
