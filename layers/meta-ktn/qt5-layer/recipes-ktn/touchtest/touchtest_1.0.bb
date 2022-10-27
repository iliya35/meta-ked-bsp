SUMMARY = "Touchtest Application"
HOMEPAGE = ""
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "file://main.cpp"
SRC_URI += "file://main.qml"
SRC_URI += "file://qml.qrc"
SRC_URI += "file://touchtest.pro"
SRC_URI += "file://touchtest.qml"

inherit qmake5
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"

DEPENDS += "qtbase qtdeclarative"
RDEPENDS:${PN} += "qtquickcontrols-qmlplugins qtbase qtbase-plugins ttf-dejavu-sans"

S = "${WORKDIR}"

# Default Values (7" Admatec)
# can be overridden in bbappends
TOUCH_WIDTH = "1024"
TOUCH_HEIGHT= "600"

do_compile() {
	# Replace values in textfile
	# add more lines like these for other parameters
	sed -i '5awidth:${TOUCH_WIDTH}' ${S}/main.qml
	sed -i '5aheight:${TOUCH_HEIGHT}' ${S}/main.qml
}

FILES:${PN} = "/opt/touchtest/* \
		 	/usr/src/debug/touchtest/* \
			"

FILES:${PN}-dbg = "/opt/touchtest/.debug*"
