SUMMARY = "QT5 gallery demo application"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://gallery.cpp;beginline=9;endline=46;md5=180eaa40e4fc7f14410f73a930ca98fe"

SRC_URI += " \
	file://autostart-app \
	file://gallery.tgz \
"

inherit qmake5 
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"

DEPENDS += " \
	qtbase \
	qtdeclarative \
	qtquickcontrols2 \
"

RDEPENDS_${PN} = "ttf-dejavu-sans"
RDEPENDS_${PN}-autostart = "${PN}"

S = "${WORKDIR}/gallery"

PACKAGES += "${PN}-autostart"

FILES_${PN} = "${bindir} ${sbindir}"
FILES_${PN}-dbg = "${bindir}/.debug ${sbindir}/.debug"
FILES_${PN}-dev = "/usr/src"
FILES_${PN}-autostart = "/etc/default/autostart-app"

do_install () {
	install -d ${D}/${bindir}
	install -Dm 0755 ${B}/gallery ${D}${bindir}

	install -d ${D}/etc/default
	install -Dm 0644 ${WORKDIR}/autostart-app ${D}${sysconfdir}/default/autostart-app
}
