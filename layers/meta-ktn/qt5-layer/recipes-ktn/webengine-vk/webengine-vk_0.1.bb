SUMMARY = "qtwebengine with included virtual keyboard."
HOMEPAGE = ""
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit qmake5 
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"
DEPENDS += "qtbase qtdeclarative qtwebengine"
RDEPENDS:${PN} += "qtwebengine-qmlplugins qtdeclarative-qmlplugins qtfreevirtualkeyboard qtquickcontrols ttf-dejavu-sans"

S = "${WORKDIR}"

SRC_URI  = "file://createHTML.h"
SRC_URI += "file://createHTML.cpp"
SRC_URI += "file://main.cpp"
SRC_URI += "file://back.png"
SRC_URI += "file://main.qml"
SRC_URI += "file://qml.qrc"
SRC_URI += "file://webengine-vk.pro"
SRC_URI += "file://refresh_google_materials.svg"


SRC_URI:append += "file://autostart-eglfs.env"
PACKAGES += "${PN}-autostart"

FILES:${PN} = 	"/opt/* \
				"
FILES:${PN}-autostart = "/etc/default/autostart-eglfs"


do_install:append() {
	install -d ${D}/etc/default
	install -Dm 0644 ${WORKDIR}/autostart-eglfs.env ${D}${sysconfdir}/default/autostart-eglfs

}
