require recipes-bsp/u-boot/u-boot-common.inc

DEPENDS += "bc-native dtc-native python3-setuptools-native"

inherit fsl-u-boot-localversion

SCMVERSION = "y"
LOCALVERSION = "_${DISTRO_CODENAME}_${DISTRO_VERSION}"
SRC_URI = "git://${KED_GIT_SERVER}/sw/misc/u-boot.git;protocol=https;branch=${SRCBRANCH}"

# For i.MX8M, depend on ATF and DDR firmware and copy binaries to U-Boot build,
# so it can be included in the SPL/FIT image.
UBOOT_IMAGE_DEPS = ""
UBOOT_IMAGE_DEPS:ktn-mx8m = "virtual/trusted-firmware-a:do_deploy firmware-imx-8m:do_deploy"
do_configure[depends] = "${UBOOT_IMAGE_DEPS}"

do_compile:prepend:ktn-mx8m() {
	for config in ${UBOOT_MACHINE}; do
		install -D -m 0644 ${DEPLOY_DIR_IMAGE}/${TFA_BINARY} ${B}/${config}/bl31.bin
	done
	for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
		install -D -m 0644 ${DEPLOY_DIR_IMAGE}/${ddr_firmware} ${B}/${config}/${ddr_firmware}
	done
}

# For i.MX8M, we need to pass the correct load address for the ATF to the
# mkimage_fit_atf.sh script, by setting the ATF_LOAD_ADDR environment variable.
EXTRA_OEMAKE:append:ktn-mx8m = " 'ATF_LOAD_ADDR=0x00920000'"
