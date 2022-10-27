DESCRIPTION = "Qt Web Browser"
LICENSE = "GPL-3.0"
LIC_FILES_CHKSUM = "file://LICENSE.GPLv3;md5=a40e2bb02b1ac431f461afd03ff9d1d6"

inherit qmake5 systemd
require recipes-qt/qt5/qt5-git.inc

QT_GIT = "git://github.com/qtproject"
QT_MODULE = "qt-apps-qtwebbrowser"
QT_MODULE_BRANCH = "dev"

SRC_URI += " \
    file://0002-qtwebbrowser-add-application-mode.patch \
"

SRCREV = "93637d1b4f32a1f73613f70b29544f232059d600"

DEPENDS = "qtbase qtdeclarative qtwebengine"

PACKAGECONFIG ?= ""
PACKAGECONFIG[desktop] = "-DDESKTOP_BUILD,,"

EXTRA_QMAKEVARS_PRE += "INSTALL_PREFIX=${bindir}"

RDEPENDS:${PN} += " \
    qtvirtualkeyboard \
    qtquickcontrols-qmlplugins \
    qtwebengine-qmlplugins \
    qtgraphicaleffects-qmlplugins \
    qtmultimedia-qmlplugins \
    ttf-dejavu-common \
    ttf-dejavu-sans \
    ttf-dejavu-sans-mono \
    ca-certificates \
"
