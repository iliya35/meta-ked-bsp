SUMMARY = "Tool to modify display settings by ddc (hdmi)"
HOMEPAGE = "http://www.ddcutil.com"
AUTHOR = "Sanford Rockowitz  <rockowitz@minsoft.com>"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

DEPENDS = "glib-2.0 libusb"
DEPENDS += "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd', 'udev', d)}"

PACKAGECONFIG:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'x11', '', d)}"

PACKAGECONFIG[x11] = "--enable-x11=yes,--enable-x11=no,"

SRCREV = "828fc4d5fd398e829f9609c56ed0e9a6e825e772"
SRC_URI = "git://github.com/rockowitz/ddcutil;protocol=https;branch=1.3.2-release"

PV = "0.9.4+${SRCPV}"

S = "${WORKDIR}/git"

inherit autotools pkgconfig
