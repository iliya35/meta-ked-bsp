SUMMARY = "Simple Qt rectangle performance test application"
HOMEPAGE = "https://github.com/fhunleth/qt-rectangles"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://README.md;md5=9db3cd593a8c5a4bf2260c22754f247d"

inherit qmake5

DEPENDS += "qtbase"

RDEPENDS:${PN} += ""

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "git://github.com/fhunleth/qt-rectangles.git;protocol=https;branch=master"
SRCBRANCH = "master"
SRCREV = "5166b6cf8ed3d5d3d3776fa8c4de913ad9f2213d"
SRC_URI[md5sum] = "5e5da3d4f8200b3ea6073b914ed62b45"
SRC_URI[sha256sum] = "06ec9e80d872ef56aa7e64e0889effd48d1fc534325b95db2b7b7c52996d3d86"

S = "${WORKDIR}/git"

FILES:${PN} = "${bindir} ${sbindir}"
FILES:${PN}-dbg = "/usr/bin/.debug"
FILES:${PN}-dev = "/usr/src"

do_install:append () {
	install -d ${D}${bindir}
	install -D -m 0755 ${B}/qt-rectangles ${D}${bindir}
}
