FILESEXTRAPATHS:prepend := "${THISDIR}/swupdate:"

inherit bootfs

SWUPDATE_IMAGES += " \
	fitImage \
	${IMAGE_BOOTFS_NAME}.tar.gz \
	${UBOOT_INITIAL_ENV}-${MACHINE} \
	${UBOOT_BINARY} \
"

SWUPDATE_IMAGES_FSTYPES[fitImage] = ""
SWUPDATE_IMAGES_NOAPPEND_MACHINE[fitImage] = "1"
