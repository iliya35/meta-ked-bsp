FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

#
# Latest linux-firmware already contains the sdsd8977_combo_v2.bin firmware file for
# the 88w8977 chipset. For usage with the latest vendor driver, we add/update the fw
# and also add the config data.
#
# FW versions from NXP package: WLAN - 16.68.10.p98, BT/BLE - 16.26.10.p98
#

LICENSE:${PN}-sd8977 = "Firmware-Marvell"
PACKAGES =+ "${PN}-sd8977"
RDEPENDS:${PN}-sd8977 += "${PN}-marvell-license"

SRC_URI += " \
    file://sd8977_ble_v2.bin      \
    file://sd8977_bt_v2.bin       \
    file://sd8977_wlan_v2.bin     \
    file://sdsd8977_combo_v2.bin  \
    file://cal_data.conf          \
    file://bt_cal_data.conf       \
    file://bt_init_cfg.conf       \
"

do_install:append() {
    cp ${WORKDIR}/sd8977* ${D}/lib/firmware/mrvl
    cp ${WORKDIR}/sdsd8977_combo_v2.bin ${D}/lib/firmware/mrvl
    cp ${WORKDIR}/cal_data.conf ${D}/lib/firmware/mrvl
    cp ${WORKDIR}/bt_cal_data.conf ${D}/lib/firmware/mrvl
    cp ${WORKDIR}/bt_init_cfg.conf ${D}/lib/firmware/mrvl
}

FILES:${PN}-sd8977 = " \
    /lib/firmware/mrvl/sd8977_ble_v2.bin       \
    /lib/firmware/mrvl/sd8977_bt_v2.bin        \
    /lib/firmware/mrvl/sd8977_wlan_v2.bin      \
    /lib/firmware/mrvl/sdsd8977_combo_v2.bin   \
    /lib/firmware/mrvl/cal_data.conf           \
    /lib/firmware/mrvl/bt_cal_data.conf        \
    /lib/firmware/mrvl/bt_init_cfg.conf        \
"
