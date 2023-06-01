SUMMARY = "Test application for qtwebengine"
HOMEPAGE = ""
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit qmake5 update-rc.d
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"
DEPENDS += "qtbase qtdeclarative qtwebengine"
RDEPENDS:${PN} += "qtwebengine-qmlplugins qtwebengine-plugins qtdeclarative-qmlplugins"

S = "${WORKDIR}"

INITSCRIPT_NAME = "autostart_webengine_test"
INITSCRIPT_PARAMS = "start 99 5 ."

SRC_URI = "file://webengine-test.pro"
SRC_URI += "file://qml.qrc"
SRC_URI += "file://MainForm.ui.qml"
SRC_URI += "file://main.qml"
SRC_URI += "file://main.cpp"
SRC_URI += "file://deployment.pri"
SRC_URI += "file://autostart_webengine_test.sh"

FILES:${PN} = 	"/usr/* \
				/etc/* \
				"

FILES:${PN}-dbg = 	"/usr/bin/.debug* \
					/usr/bin/${PN}/.debug* \
		  			"

do_install:append() {
	install -D -m 0755 ${S}/autostart_webengine_test.sh ${D}${sysconfdir}/init.d/autostart_webengine_test
}
