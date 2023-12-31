require recipes-bsp/u-boot/u-boot-common.inc

DEPENDS += "bc-native dtc-native python3-setuptools-native"

inherit fsl-u-boot-localversion

SCMVERSION = "y"
LOCALVERSION = "_${DISTRO_CODENAME}_${DISTRO_VERSION}"
SRC_URI = "git://${KED_GIT_SERVER}/sw/misc/u-boot.git;protocol=https;branch=${SRCBRANCH}"

# For i.MX8M, depend on ATF and DDR firmware and copy binaries to U-Boot build,
# so it can be included in the SPL/FIT image.
UBOOT_IMAGE_DEPS = ""
UBOOT_IMAGE_DEPS:ked-mx8m = "virtual/trusted-firmware-a:do_deploy firmware-imx-8m:do_deploy"
do_configure[depends] = "${UBOOT_IMAGE_DEPS}"

do_compile:prepend:ked-mx8m() {
	for config in ${UBOOT_MACHINE}; do
		install -D -m 0644 ${DEPLOY_DIR_IMAGE}/${TFA_BINARY} ${B}/${config}/bl31.bin
	done
	for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
		install -D -m 0644 ${DEPLOY_DIR_IMAGE}/${ddr_firmware} ${B}/${config}/${ddr_firmware}
	done
}

# For i.MX8M, we need to pass the correct load address for the ATF to the
# mkimage_fit_atf.sh script, by setting the ATF_LOAD_ADDR environment variable.
EXTRA_OEMAKE:append:ked-mx8m = " 'ATF_LOAD_ADDR=0x00920000'"

# Code snippet from poky linux-yocto.inc to fix menuconfig presentation
do_devshell:prepend() {
    # setup native pkg-config variables (kconfig scripts call pkg-config directly, cannot generically be overriden to pkg-config-native)
    d.setVar("PKG_CONFIG_DIR", "${STAGING_DIR_NATIVE}${libdir_native}/pkgconfig")
    d.setVar("PKG_CONFIG_PATH", "${PKG_CONFIG_DIR}:${STAGING_DATADIR_NATIVE}/pkgconfig")
    d.setVar("PKG_CONFIG_LIBDIR", "${PKG_CONFIG_DIR}")
    d.setVarFlag("PKG_CONFIG_SYSROOT_DIR", "unexport", "1")
    d.appendVar("OE_TERMINAL_EXPORTS", " PKG_CONFIG_DIR PKG_CONFIG_PATH PKG_CONFIG_LIBDIR PKG_CONFIG_SYSROOT_DIR")
}
