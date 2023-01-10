FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# v2.4
SRCREV_tfa = "e2c509a39c6cc4dda8734e6509cdbe6e3603cdfc"

COMPATIBLE_MACHINE = "(mx8m)"

EXTRA_OEMAKE:append:mx8m = " \
    IMX_BOOT_UART_BASE="0x30880000"	\
"

SRC_URI:append = " \
	file://0001-plat-imx8m-update-the-wdog-config-for-system-reset.patch \
"
