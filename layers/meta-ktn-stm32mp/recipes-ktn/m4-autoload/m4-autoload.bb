FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DESCRIPTION = "installs autostart service for m4 firmware "

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "\
	file://m4-autoload.service \
	file://m4-autoload.sh \
	file://m4-autoload.conf \
	file://binary/m4-application.elf \
	file://binary/m4_ktn-simple-monitor.elf \
	file://binary/m4_ktn-eval-usart6.elf \
	file://binary/m4_ktn_echo_CM4.elf \
	file://rpmsg0 \
	file://rpmsg1 \
	"

PROJECT_LIST = "\
	m4-application.elf \
	m4_ktn-simple-monitor.elf \
	m4_ktn-eval-usart6.elf	\
	m4_ktn_echo_CM4.elf \
	"

S="${WORKDIR}"

SYSTEMD_SERVICE:${PN} = "m4-autoload.service"

PACKAGES += " m4-demos"
RDEPENDS:m4-demos = "${PN}"

INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME:${PN} = "m4-autoload.sh"
INITSCRIPT_PARAMS:${PN} = "start 1 5 2 ."

APPLICATION_PATH = "/usr/share/m4-application"

FILES:${PN} = "\
	${systemd_unitdir} \
	${bindir}/m4-autoload \
	${sysconfdir} \
	${APPLICATION_PATH}/.dummy \
	"

FILES:m4-demos = "\
	${APPLICATION_PATH} \
	${bindir}/rpmsg* \
	"

inherit update-rc.d systemd

do_install () {

	# init scripts
	install -d ${D}${sysconfdir}/default
	install -Dm 0644 ${WORKDIR}/m4-autoload.conf ${D}${sysconfdir}/default/m4-autoload.conf
	install -d ${D}${sysconfdir}/init.d
	install -c -m 755 ${WORKDIR}/m4-autoload.sh ${D}${sysconfdir}/init.d/m4-autoload.sh

	install -d ${D}${bindir}
	install -Dm 0755 ${WORKDIR}/m4-autoload.sh ${D}${bindir}/m4-autoload
	install -d ${D}${systemd_system_unitdir}
	install -Dm 0644 ${WORKDIR}/m4-autoload.service ${D}${systemd_system_unitdir}/m4-autoload.service

	install -d ${D}${APPLICATION_PATH}
	touch ${D}${APPLICATION_PATH}/.dummy

	# demo files
	for project in ${PROJECT_LIST} ; do
		install -m 0755 ${WORKDIR}/binary/${project} ${D}${APPLICATION_PATH}/${project}
	done
	install -c -m 755 ${WORKDIR}/rpmsg* ${D}${bindir}

}


