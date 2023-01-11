SUMMARY = "qtwebengine with included virtual keyboard."
HOMEPAGE = ""
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit qmake5 
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"
DEPENDS += "qtbase qtdeclarative qtwebengine"
RDEPENDS:${PN} += "qtwebengine-qmlplugins qtdeclarative-qmlplugins qtfreevirtualkeyboard qtquickcontrols ttf-dejavu-sans"

S = "${WORKDIR}"

SRC_URI  = " \
	file://autostart-eglfs.env \
	file://back.png \
	file://createHTML.cpp \
	file://createHTML.h \
	file://main.cpp \
	file://main.qml \
	file://qml.qrc \
	file://refresh_google_materials.svg \
	file://webengine-vk.pro \
"

PACKAGES += "${PN}-autostart"

FILES:${PN} = 	"/opt/* "
FILES:${PN}-autostart = "/etc/default/autostart-eglfs"

do_install:append() {
	install -d ${D}/etc/default
	install -Dm 0644 ${WORKDIR}/autostart-eglfs.env ${D}${sysconfdir}/default/autostart-eglfs
}
