DESCRIPTION = "installs example pictures on image"

LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI += "file://flowerfields-480x800.png"
SRC_URI += "file://flowerfields-800x480.png"
SRC_URI += "file://flowerfields-1024x600.png"
FILES:${PN} = "/usr/share/images/*"

do_install () {
    mkdir -p ${D}/usr/share/images
    install -D -m 0755 ${WORKDIR}/*.png	${D}/usr/share/images
}
