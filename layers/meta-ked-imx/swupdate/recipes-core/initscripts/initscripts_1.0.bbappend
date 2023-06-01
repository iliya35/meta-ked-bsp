FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://reset-update-flags.sh"

do_install:append () {
	install -m 0755    ${WORKDIR}/reset-update-flags.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} reset-update-flags.sh start 99 5 . 
}
