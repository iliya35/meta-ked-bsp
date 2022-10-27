DESCRIPTION = "SWUpdate compound image for console image" 

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit swupdate

#SWUPDATE_SIGNING = "RSA"
#SWUPDATE_PRIVATE_KEY = "${THISDIR}/files/stm32mp-t1000/example-swupdate-priv.pem"

SRC_URI = "\
	file://sw-description \
	file://pre_post_script.sh \
	"

# images to build before building swupdate image
# for now dont build this all the time
IMAGE_DEPENDS = "image-ktn-swu"

# images and files that will be included in the .swu image
SWUPDATE_IMAGES = "\
	image-ktn-swu \
	image-ktn-swu-bootfiles \
	"

SWUPDATE_IMAGES_FSTYPES[image-ktn-swu] = ".tar.gz"
SWUPDATE_IMAGES_NOAPPEND_MACHINE[image-ktn-swu] = "0"
SWUPDATE_IMAGES_FSTYPES[image-ktn-swu-bootfiles] = ".tar.gz"
SWUPDATE_IMAGES_NOAPPEND_MACHINE[image-ktn-swu-bootfiles] = "0"
