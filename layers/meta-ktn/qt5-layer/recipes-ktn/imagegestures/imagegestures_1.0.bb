SUMMARY = "A Qt5 Demo Widget Application"
HOMEPAGE = ""
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit qmake5
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"
DEPENDS += "qtbase"
RDEPENDS:${PN} += "ttf-dejavu-sans"

S = "${WORKDIR}/ImageGestures"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRCBRANCH = "master"
SRCREV = "972776a3c57673311c05f6a5f941d531b268fcda"

SRC_URI = "git://${KTN_GIT_APPS}/widgets-demo.git;protocol=https;branch=${SRCBRANCH};subpath=ImageGestures"

FILES:${PN} =  "/usr/bin/* \
                /usr/share/imagegestures* \
                "

FILES:${PN}-dbg = "/usr/bin/.debug*"
FILES:${PN}-dev = "/usr/src"
