SUMMARY = "The Qt5 Demo QML Application"
HOMEPAGE = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENCE.txt;md5=1d5f292b3aa4f78ee13e4239b3b5a885"

inherit qmake5 
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"

DEPENDS += "qtbase qtdeclarative qtmultimedia qtwebengine"

RDEPENDS:${PN} += "qtxmlpatterns qtdeclarative-qmlplugins qtbase-plugins ttf-dejavu-sans qtquickcontrols qtquickcontrols2 qtfreevirtualkeyboard"

RDEPENDS:${PN}-video += "gstreamer1.0-plugins-base qtmultimedia"
RDEPENDS:${PN}-video:append:imx += "gstreamer1.0-plugins-imx"
RDEPENDS:${PN}-video:remove:use-mainline-bsp = "gstreamer1.0-plugins-imx"
RDEPENDS:${PN}-autostart += "autostart-app"

S = "${WORKDIR}/git"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRCBRANCH = "master"
SRCREV = "0450237684cdd7333c113ee7e0bc7558e81c6b29"

SRC_URI = "git://${KTN_GIT_APPS}/kontron-demo.git;protocol=https;branch=${SRCBRANCH}"
SRC_URI:append += " file://video/*"
SRC_URI:append += " file://slideshow/*"
SRC_URI:append += " file://autostart-app.env"

PACKAGES += "${PN}-autostart ${PN}-video ${PN}-slideshow"

FILES:${PN} = "/usr/bin/kontron-demo"
FILES:${PN}-dbg = "/opt/bin/.debug*"
FILES:${PN}-video = "/usr/share/kontron-demo/video"
FILES:${PN}-slideshow = "/usr/share/kontron-demo/slideshow"
FILES:${PN}-autostart = "/etc/default/autostart-app"

FILES:${PN}-dev = "/usr/src"

do_install:append () {
	install -d ${D}/usr/share/kontron-demo/video
	install -D -m 0755 ${WORKDIR}/video/* ${D}${datadir}/kontron-demo/video/

	install -d ${D}/usr/share/kontron-demo/slideshow
	install -D -m 0755 ${WORKDIR}/slideshow/* ${D}${datadir}/kontron-demo/slideshow/
	
	install -d ${D}/etc/default
	install -Dm 0644 ${WORKDIR}/autostart-app.env ${D}${sysconfdir}/default/autostart-app
}
