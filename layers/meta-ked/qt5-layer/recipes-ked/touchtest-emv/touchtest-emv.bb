SUMMARY = "The Qt5 EMV Application for Displayline products"
HOMEPAGE = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d6e8e6dc7b4865368a048aef36b0676e"


inherit qmake5 update-rc.d
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"

DEPENDS += "qtbase qtdeclarative"

RDEPENDS:${PN} += "qtdeclarative-qmlplugins qtbase-plugins ttf-dejavu-sans"
S = "${WORKDIR}/git"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRCBRANCH = "master"
SRCREV = "8b68dc8e4ab4604fd3a622837cd4732057e97832"

SRC_URI = " \
	git://${KED_GIT_APPS}/touchtest-emv.git;protocol=http;branch=${SRCBRANCH} \
	file://start-touchtest-emv.sh \
	file://settings-touchtest-emv \
"

PACKAGES += "${PN}-autostart"

INITSCRIPT_PACKAGES = "${PN}-autostart"
INITSCRIPT_NAME:${PN}-autostart = "start-touchtest-emv"
INITSCRIPT_PARAMS:${PN}-autostart = "start 99 5 ."

FILES:${PN} = "/usr/bin/touchtest-emv"
FILES:${PN}-dbg = "/opt/touchtest-emv/.debug*"
FILES:${PN}-autostart = "/etc/init.d/*  /etc/default/*"

do_install:append () {
	install -d ${D}${sysconfdir}/init.d
	install -D -m 0755 ${WORKDIR}/start-touchtest-emv.sh ${D}${sysconfdir}/init.d/start-touchtest-emv

	install -d ${D}/etc/default
	install -D -m 0755 ${WORKDIR}/settings-touchtest-emv ${D}${sysconfdir}/default/settings-touchtest-emv
}
