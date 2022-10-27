SUMMARY = "Tool to modify display settings by ddc (hdmi)"
HOMEPAGE = "http://www.ddcutil.com"
AUTHOR = "Sanford Rockowitz  <rockowitz@minsoft.com>"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

DEPENDS = "glib-2.0 libusb"
DEPENDS += "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd', 'udev', d)}"

PACKAGECONFIG:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'x11', '', d)}"

PACKAGECONFIG[x11] = "--enable-x11=yes,--enable-x11=no,"

SRCREV = "1834de60e8658c3c86f9d1e27b1e94830ffcab27"
SRC_URI = "git://github.com/rockowitz/ddcutil;protocol=git"

PV = "0.9.4+${SRCPV}"

S = "${WORKDIR}/git"

inherit autotools pkgconfig
