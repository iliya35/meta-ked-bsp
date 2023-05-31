SUMMARY = "Packagegroup for swupdate"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit packagegroup

PACKAGES = "\
		packagegroup-update-base \
		"
		
RDEPENDS:packagegroup-update-base = "\
		swupdate-extra-files \
		swupdate \
		swupdate-www \
		swupdate-client \
		"

