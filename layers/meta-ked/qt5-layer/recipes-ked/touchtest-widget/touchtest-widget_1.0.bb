SUMMARY = "Touchtest Widget Application"
HOMEPAGE = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRCBRANCH = "master"
SRCREV = "d7ea2bab405cd82241619a46362e3f5fe83c7d2c"

SRC_URI = "git://${KED_GIT_APPS}/touchtest-widget.git;protocol=http;branch=${SRCBRANCH}"
SRC_URI += "file://touchtest-widget-autostart"

inherit qmake5 update-rc.d

PACKAGES += "${PN}-autostart"

INITSCRIPT_PACKAGES = "${PN}-autostart"
INITSCRIPT_NAME:${PN}-autostart = "touchtest-widget-autostart"
INITSCRIPT_PARAMS:${PN}-autostart = "start 99 5 ."

OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"

DEPENDS += "qtbase"
RDEPENDS:${PN} += "ttf-dejavu-sans"

S = "${WORKDIR}/git"

FILES:${PN} = "/usr/bin/touchtest-widget"
FILES:${PN}-autostart = "/etc/init.d/touchtest-widget-autostart"

do_install:append () {
	install -d ${D}${sysconfdir}/init.d
	install -D -m 0755 ${WORKDIR}/touchtest-widget-autostart ${D}${sysconfdir}/init.d/touchtest-widget-autostart
}
