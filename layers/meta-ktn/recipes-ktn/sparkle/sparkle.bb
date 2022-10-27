DESCRIPTION = "Kontron EMV test application with web interface for client and server part"
HOMEPAGE = "https://git.kontron-electronics.de/apps/sparkle"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d6e8e6dc7b4865368a048aef36b0676e"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

FILES:${PN} = "/usr/share/sparkle/* /etc/init.d/* /usr/bin/* "
 
S = "${WORKDIR}/git"

SRC_URI = "git://${KTN_GIT_SERVER_SSH}/sw/misc/apps/sparkle.git;protocol=ssh;branch=${SRCBRANCH}"
#SRCREV = "0b2fc0466aa05626e6721d8942b5e0b35cb1b062"
SRCREV = "rel_1.9.0"
SRCBRANCH = "master"

SRC_URI[md5sum] = "8d340c2a203dc3a2a3e912a9d681b311"
SRC_URI[sha256sum] = "54672b7af153479e586ef1c48dc1514695e36fc98781bd4a52b44edcbe471dd5"

# Add default configuration files
SRC_URI:append += "file://sparklesrv.default"
SRC_URI:append += "file://sparkle.cfg"
SRC_URI:append += "file://sparkle-dut-mx8mm.cfg"
SRC_URI:append += "file://sparkle-dut-mx6d.cfg"
SRC_URI:append += "file://sparkle-dut-mx6ul-combox.cfg"


inherit setuptools3 update-rc.d systemd

PACKAGES += "${PN}-autostart"

DEPENDS = "python3"
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

INITSCRIPT_NAME = "sparklesrv.sh"
INITSCRIPT_PARAMS = "start 50 5 2 . "

SYSTEMD_SERVICE:${PN} = "sparklesrv.service"

do_install:append() {
	install -d ${D}/etc/default
	install -m 0755 ${WORKDIR}/sparklesrv.default ${D}/etc/default/sparklesrv
	install -m 0755 ${WORKDIR}/sparkle.cfg ${D}/etc/sparkle.cfg
	install -m 0755 ${WORKDIR}/sparkle-dut-mx8mm.cfg ${D}/etc/sparkle-dut-mx8mm.cfg
	install -m 0755 ${WORKDIR}/sparkle-dut-mx6d.cfg ${D}/etc/sparkle-dut-mx6d.cfg
	install -m 0755 ${WORKDIR}/sparkle-dut-mx6ul-combox.cfg ${D}/etc/sparkle-dut-mx6ul-combox.cfg
}
