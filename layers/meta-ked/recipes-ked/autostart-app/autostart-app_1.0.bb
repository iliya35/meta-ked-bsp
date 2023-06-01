FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

DESCRIPTION = "Common start script for graphical Qt applications"

LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "\
	file://autostart-app.service \
	file://autostart-app.sh \
	file://autostart-app.sample \
	file://autostart-app-initscript.sh \
	"

inherit allarch features_check
CONFLICT_DISTRO_FEATURES = "wayland"

INITSCRIPT_NAME = "autostart-app-initscript.sh"
INITSCRIPT_PARAMS = "start 99 5 2 . "

inherit update-rc.d systemd

S="${WORKDIR}"
SYSTEMD_SERVICE:${PN} = "autostart-app.service"

do_install () {
	install -Dm 0644 ${WORKDIR}/autostart-app.sample ${D}${sysconfdir}/default/autostart-app.sample
	install -Dm 0755 ${WORKDIR}/autostart-app.sh ${D}${bindir}/autostart-app.sh

	if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
		install -Dm 0644 ${WORKDIR}/autostart-app.service ${D}${systemd_system_unitdir}/autostart-app.service
	else
		install -d ${D}${sysconfdir}/init.d/
		install -c -m 755 ${WORKDIR}/${INITSCRIPT_NAME} ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
	fi
}
