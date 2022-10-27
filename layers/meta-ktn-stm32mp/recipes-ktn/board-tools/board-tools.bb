FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "convenience tools for board peripherals"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"


PV = "1"
PR = "r0"

SRC_URI = "\
	file://adcread \
	"

inherit allarch

S="${WORKDIR}"

do_install () {
    install -Dm 0755 ${WORKDIR}/adcread ${D}${bindir}/adcread
}
