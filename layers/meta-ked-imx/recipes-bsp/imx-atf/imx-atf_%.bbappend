FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

require imx-atf-ktn-common.inc

ATF_PLATFORM = "${TFA_PLATFORM}"
# Dunfell compat
PLATFORM = "${TFA_PLATFORM}"

COMPATIBLE_MACHINE = "(ked-imx|imx-mainline-bsp)"
