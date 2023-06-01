FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

python () {
    d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", d.getVar("IMAGE_LINK_NAME"), ".tar.gz")
    d.setVarFlag("SWUPDATE_IMAGES_NOAPPEND_MACHINE", d.getVar("IMAGE_LINK_NAME"), "1")
}

inherit swupdate-image

SRC_URI += "file://pre_post_script.sh"

IMAGE_INSTALL += "packagegroup-update-base"
