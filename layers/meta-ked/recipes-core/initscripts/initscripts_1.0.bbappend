FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:ked-mx6ul = " file://change-if.sh"

do_install:append:ked-mx6ul () {
	install -m 0755    ${WORKDIR}/change-if.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} change-if.sh start 01 2 3 4 5 .
}
