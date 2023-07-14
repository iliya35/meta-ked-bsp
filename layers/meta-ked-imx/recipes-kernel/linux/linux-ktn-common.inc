SUMMARY = "The Linux kernel with Kontron patches and hardware support"
DESCRIPTION = ""
LICENSE = "GPL-2.0-only"

inherit kernel

SCMVERSION = "y"

# Set CONFIG_LOCALVERSION
LINUX_VERSION_EXTENSION ?= "-ktn"
do_compile:prepend() {
	echo "CONFIG_LOCALVERSION="\"${LINUX_VERSION_EXTENSION}\" >> ${B}/.config
}

SRC_URI = " \
	git://${KED_GIT_SERVER}/sw/misc/linux.git;protocol=https;branch=${SRCBRANCH} \
	file://defconfig \
"

S = "${WORKDIR}/git"

# Fix missing gmp.h issues on kernel >= 5.8
DEPENDS += "gmp-native"
EXTRA_OEMAKE += "HOSTCXX="${BUILD_CXX} ${BUILD_CXXFLAGS} ${BUILD_LDFLAGS}""

# We need to pass it as param since kernel might support more then one
# machine, with different entry points
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"
KERNEL_DTC_FLAGS += "-@"

DEPENDS += "lzop-native bc-native"

KBRANCH = "${SRCBRANCH}"

COMPATIBLE_MACHINE = "(ked-imx|imx-mainline-bsp)"

# Code snippet from poky linux-yocto.inc to fix menuconfig presentation
do_devshell:prepend() {
    # setup native pkg-config variables (kconfig scripts call pkg-config directly, cannot generically be overriden to pkg-config-native)
    d.setVar("PKG_CONFIG_DIR", "${STAGING_DIR_NATIVE}${libdir_native}/pkgconfig")
    d.setVar("PKG_CONFIG_PATH", "${PKG_CONFIG_DIR}:${STAGING_DATADIR_NATIVE}/pkgconfig")
    d.setVar("PKG_CONFIG_LIBDIR", "${PKG_CONFIG_DIR}")
    d.setVarFlag("PKG_CONFIG_SYSROOT_DIR", "unexport", "1")
    d.appendVar("OE_TERMINAL_EXPORTS", " PKG_CONFIG_DIR PKG_CONFIG_PATH PKG_CONFIG_LIBDIR PKG_CONFIG_SYSROOT_DIR")
}
