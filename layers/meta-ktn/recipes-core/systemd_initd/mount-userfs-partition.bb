DESCRIPTION = "Mount userfs partitions"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# This recipe is intended to be used with mount-userfs.bbclass

RDEPENDS:${PN} += " util-linux "

SRC_URI = " \
    file://${BPN}.service    \
    file://${BPN}.sh         \
    "

inherit systemd update-rc.d userfs-settings

INITSCRIPT_NAME = "${PN}.sh"
INITSCRIPT_PARAMS = "start 22 5 3 ."

SYSTEMD_PACKAGES = "${@bb.utils.contains('DISTRO_FEATURES','systemd','${PN}','',d)}"
SYSTEMD_SERVICE:${PN} = "${PN}.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install() {

    # write files
    install -d ${D}${systemd_unitdir}/system
    install -d ${D}${base_sbindir}
    install -m 644 ${WORKDIR}/${PN}.service ${D}${systemd_unitdir}/system
    install -m 755 ${WORKDIR}/${PN}.sh ${D}${base_sbindir}/

    install -d ${D}${sysconfdir}/init.d
    install -m 755 ${WORKDIR}/${PN}.sh ${D}${sysconfdir}/init.d
}

FILES:${PN} += "${systemd_unitdir} ${base_sbindir} ${sysconfdir}/init.d"
