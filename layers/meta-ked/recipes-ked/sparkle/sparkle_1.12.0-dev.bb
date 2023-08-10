DESCRIPTION = "Kontron EMV test application with web interface for client and server part"
HOMEPAGE = "https://git.kontron-electronics.de/apps/sparkle"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d6e8e6dc7b4865368a048aef36b0676e"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

FILES:${PN} = "/usr/share/sparkle/* /etc/init.d/* /usr/bin/* "
 
S = "${WORKDIR}/git"

SRC_URI = " \
     git://${KED_GIT_SERVER_SSH}/sw/misc/apps/sparkle.git;protocol=ssh;branch=${SRCBRANCH} \
     file://sparklesrv.default \
     file://sparkle.cfg \
     file://sparkle-dut-mx8mm.cfg \
     file://sparkle-dut-mx6d.cfg \
     file://sparkle-dut-mx6ul-combox.cfg \
"

SRCREV = "9ef635aab9dbdc557b5612f0873301e45314758f"
SRCBRANCH = "master"

SRC_URI[md5sum] = "8d340c2a203dc3a2a3e912a9d681b311"
SRC_URI[sha256sum] = "54672b7af153479e586ef1c48dc1514695e36fc98781bd4a52b44edcbe471dd5"

SETUPTOOLS = "setuptools3_legacy"
SETUPTOOLS:dunfell = "setuptools3"

inherit ${SETUPTOOLS}
inherit update-rc.d systemd

PACKAGES += "${PN}-autostart"

DEPENDS = "python3 python3-setuptools-native"
RDEPENDS:${PN} = " \
     python3-websockets \
     python3-pyserial \
     python3-can \
     python3-wrapt \ 
     python3-setuptools \
     libgpiod \
     libgpiod-python \
"
RDEPENDS:${PN}-autostart = "${PN}"

FILES:${PN} = "/bin /usr /etc/sparkle.cfg "
FILES:${PN}-autostart += " /etc "

INITSCRIPT_PACKAGES = "${PN}-autostart"
INITSCRIPT_NAME_${PN}-autostart = "sparklesrv.sh"
INITSCRIPT_PARAMS_${PN}-autostart = "start 50 5 2 . "

SYSTEMD_SERVICE:${PN} = "sparklesrv.service"

do_install:append() {
	install -d ${D}/etc/default
	install -m 0755 ${WORKDIR}/sparklesrv.default ${D}/etc/default/sparklesrv
	install -m 0755 ${WORKDIR}/sparkle.cfg ${D}/etc/sparkle.cfg
	install -m 0755 ${WORKDIR}/sparkle-dut-mx8mm.cfg ${D}/etc/sparkle-dut-mx8mm.cfg
	install -m 0755 ${WORKDIR}/sparkle-dut-mx6d.cfg ${D}/etc/sparkle-dut-mx6d.cfg
	install -m 0755 ${WORKDIR}/sparkle-dut-mx6ul-combox.cfg ${D}/etc/sparkle-dut-mx6ul-combox.cfg
}
