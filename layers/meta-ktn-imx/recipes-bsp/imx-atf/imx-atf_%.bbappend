FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Change deployment paths to match that of the mainline TF-A
do_deploy() {
    install -Dm 0644 ${S}/build/${PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${TFA_BINARY}
    if ${BUILD_OPTEE}; then
       install -m 0644 ${S}/build-optee/${PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${TFA_BINARY}-optee
    fi
}

PROVIDES = "virtual/trusted-firmware-a"

SRC_URI:append:ktn-mx8mm = " \
    file://0001-i.MX8MM-Change-debug-UART-from-UART2-to-UART3.patch \
    file://0001-imx8mm_bl31_setup.c-Assign-UART4-to-A53-instead-of-M.patch \
"

SRCBRANCH = "imx_5.4.70_2.3.0"
SRCREV = "f1d7187f261ebf4b8a2a70d638d4bfc0a9b26c29"

SRC_URI:remove = " \
    file://0001-imx-Fix-missing-inclusion-of-cdefs.h.patch      \
    file://0001-imx-Fix-multiple-definition-of-ipc_handle.patch \
"
