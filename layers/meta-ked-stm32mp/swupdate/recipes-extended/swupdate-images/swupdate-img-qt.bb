DESCRIPTION = "SWUpdate compound image for Qt5 image" 

LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit swupdate

#SWUPDATE_SIGNING = "RSA"
#SWUPDATE_PRIVATE_KEY = "${THISDIR}/files/stm32mp-t1000/example-swupdate-priv.pem"

SRC_URI = "\
	file://sw-description \
	file://pre_post_script.sh \
	"

# images to build before building swupdate image
# for now dont build this all the time
IMAGE_DEPENDS = "image-ked-qt-swu"

# images and files that will be included in the .swu image
SWUPDATE_IMAGES = "\
	image-ked-qt-swu \
	image-ked-qt-swu-bootfiles \
	"

SWUPDATE_IMAGES_FSTYPES[image-ked-qt-swu] = ".tar.gz"
SWUPDATE_IMAGES_NOAPPEND_MACHINE[image-ked-qt-swu] = "0"
SWUPDATE_IMAGES_FSTYPES[image-ked-qt-swu-bootfiles] = ".tar.gz"
SWUPDATE_IMAGES_NOAPPEND_MACHINE[image-ked-qt-swu-bootfiles] = "0"
