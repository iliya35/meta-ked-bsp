FILESEXTRAPATHS:append := "${THISDIR}/files:"

DESCRIPTION = "add config file for updates to userfs"

LICENSE = "GPLv2"
#LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.GPLv2;md5=751419260aa954499f7abaabaa882bbe"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI += "\
	file://example-swupdate-public.pem \
	"
FILES:${PN} += "usr/local/swupdate-files/* \
				"


do_install:append () {

	install -d ${D}/usr/local/swupdate-files
   	install -m 755 ${WORKDIR}/example-swupdate-public.pem ${D}/usr/local/swupdate-files/swupdate-public.pem
}
