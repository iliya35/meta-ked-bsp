SUMMARY = ""
HOMEPAGE = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

inherit qmake5
OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"
DEPENDS += "qtbase qtdeclarative"
RDEPENDS:${PN} += "qtquickcontrols-qmlplugins qtgraphicaleffects-qmlplugins"

S = "${WORKDIR}/git/src"

SRCBRANCH = "master"
SRCREV = "c025c9963345115058534ec97aab561fbeb63a7c"

SRC_URI[md5sum] = "f57c9a9b92a00e9946a663915022a198"
SRC_URI[sha256sum] = "8d8c85d6e83a80ffa03e7c0acf7f1288e11a6c6ad545d79a4c04f3d39bf29ece"

SRC_URI = "git://${KED_GIT_APPS}/QtFreeVirtualKeyboard.git;protocol=https;branch=${SRCBRANCH}"

FILES:${PN} = "/usr/lib"
