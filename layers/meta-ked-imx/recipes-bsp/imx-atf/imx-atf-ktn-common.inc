# Change deployment paths to match that of the mainline TF-A
do_deploy() {
    install -Dm 0644 ${S}/build/${TFA_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${TFA_BINARY}
    if ${BUILD_OPTEE}; then
        install -m 0644 ${S}/build-optee/${TFA_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${TFA_BINARY}-optee
    fi
}

PROVIDES = "virtual/trusted-firmware-a"

SRC_URI:append:ked-mx8mm = " \
    file://0001-imx8mm_bl31_setup.c-Assign-UART4-to-A53-instead-of-M.patch \
"

COMPATIBLE_MACHINE = "(ked-imx|imx-mainline-bsp)"
