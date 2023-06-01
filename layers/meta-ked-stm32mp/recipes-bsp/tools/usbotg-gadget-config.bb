# Copyright (C) 2018, STMicroelectronics - All Rights Reserved
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "The goal is to enable USB gadget configuration"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

PV = "1.0"

SRC_URI = " file://usbotg-config.service \
    file://stm32_usbotg_eth_config.sh \
    file://53-usb-otg.network \
    "

S = "${WORKDIR}/git"

inherit systemd update-rc.d

SYSTEMD_PACKAGES += "${PN}"
SYSTEMD_SERVICE:${PN} = "usbotg-config.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install() {
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system ${D}${base_sbindir} ${D}${systemd_unitdir}/network
        install -m 0644 ${WORKDIR}/usbotg-config.service ${D}${systemd_unitdir}/system
        install -m 0755 ${WORKDIR}/stm32_usbotg_eth_config.sh ${D}${base_sbindir}
        install -m 0644 ${WORKDIR}/53-usb-otg.network ${D}${systemd_unitdir}/network
    fi

    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/stm32_usbotg_eth_config.sh ${D}${sysconfdir}/init.d/

}

INITSCRIPT_NAME = "stm32_usbotg_eth_config.sh"
INITSCRIPT_PARAMS = "start 22 5 3 ."

FILES:${PN} += "${systemd_unitdir}/network"

