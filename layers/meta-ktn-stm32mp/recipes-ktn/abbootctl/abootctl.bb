DESCRIPTION = ""
HOMEPAGE = ""
LICENSE = "GPLv2+ & LGPLv2+"
LIC_FILES_CHKSUM = "file://LICENSE;md5=940fce359150d24680770a58a7691669"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

FILES:${PN} = "/etc/default/* /etc/init.d/* /usr/bin/* "
 
S = "${WORKDIR}/git"

SRC_URI = " \
     git://${KTN_GIT_SERVER_SSH}/sw/misc/apps/abootctl.git;protocol=ssh;branch=${SRCBRANCH} \
     file://abootctl.sh \
     file://abootctl.default \
"

SRCREV = "944581d01e15d1cda4efa6b7fd8c143e906842c3"
SRCBRANCH = "develop"

inherit cmake update-rc.d systemd

PACKAGES += "${PN}-autostart"

RDEPENDS:${PN}-autostart = "${PN}"

FILES:${PN} = "/bin /usr"
FILES:${PN}-autostart += " /etc "

INITSCRIPT_NAME = "abootctl.sh"
INITSCRIPT_PARAMS = "start 27 5 2 . "

#SYSTEMD_SERVICE:${PN} = "abootctl.service"

do_install:append() {
	install -d ${D}/etc/default
	install -m 0644 ${WORKDIR}/abootctl.default ${D}/etc/default/abootctl

	install -d ${D}/etc/init.d
     install -m 0755 ${WORKDIR}/abootctl.sh ${D}/etc/init.d/abootctl.sh
}
