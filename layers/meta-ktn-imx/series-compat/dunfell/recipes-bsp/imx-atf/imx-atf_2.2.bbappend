FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:remove = " \
    file://0001-imx-Fix-missing-inclusion-of-cdefs.h.patch      \
    file://0001-imx-Fix-multiple-definition-of-ipc_handle.patch \
"

SRC_URI:append:ktn-mx8mm = " \
    file://0001-i.MX8MM-Change-debug-UART-from-UART2-to-UART3.patch \
"

SRCBRANCH = "imx_5.4.70_2.3.0"
SRCREV = "f1d7187f261ebf4b8a2a70d638d4bfc0a9b26c29"

PLATFORM = "${TFA_PLATFORM}"
COMPATIBLE_MACHINE = "(ktn-mx8m)"
