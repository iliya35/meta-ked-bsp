DESCRIPTION = "Kontron stress test application"
HOMEPAGE = "https://git.kontron-electronics.de/"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=f0ef55c0271bf2dd76834e4371f3e3c7"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

FILES:${PN} = "/usr/share/sparkle/* /etc/init.d/* /usr/bin/* "
 
SRC_URI = "file://src"

S = "${WORKDIR}/src"

inherit update-rc.d allarch

PACKAGES += "${PN}-autostart"

RDEPENDS:${PN} = " \
     stress-ng \
     memtester \
"
RDEPENDS:${PN}-autostart = "${PN}"

FILES:${PN} = "/usr /etc/ktnstress.conf "
FILES:${PN}-autostart += " /etc "

INITSCRIPT_NAME = "ktnstressd.sh"
INITSCRIPT_PARAMS = "start 80 5 2 . "

do_install:append() {
     install -d ${D}/etc/default
     install -m 0755 ${S}/ktnstress.conf ${D}/etc/ktnstress.conf
     install -d ${D}/etc/init.d
     install -m 0755 ${S}/ktnstressd.sh ${D}/etc/init.d/ktnstressd.sh
     install -d ${D}/${bindir}
     install -m 0755 ${S}/ktnstress.sh ${D}/${bindir}/ktnstress.sh
}
