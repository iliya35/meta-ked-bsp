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
