SUMMARY = "The Linux kernel with Kontron patches and hardware support"
DESCRIPTION = ""
LICENSE = "GPLv2"

inherit kernel

SCMVERSION = "y"

# Set CONFIG_LOCALVERSION
LINUX_VERSION_EXTENSION ?= "-ktn"
do_compile:prepend() {
	echo "CONFIG_LOCALVERSION="\"${LINUX_VERSION_EXTENSION}\" >> ${B}/.config
}

SRC_URI = " \
	git://${KTN_GIT_SERVER}/sw/misc/linux.git;protocol=https;branch=${SRCBRANCH} \
	file://defconfig \
"

S = "${WORKDIR}/git"

# We need to pass it as param since kernel might support more then one
# machine, with different entry points
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

DEPENDS += "lzop-native bc-native"

KBRANCH = "${SRCBRANCH}"

COMPATIBLE_MACHINE = "(imx)"
