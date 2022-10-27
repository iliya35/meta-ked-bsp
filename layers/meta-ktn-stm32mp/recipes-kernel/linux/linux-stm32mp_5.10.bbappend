FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}-${LINUX_VERSION}:"

# OpenST v5.10-stm32mp-r2.1 + KTN Patches
# Based on OpenST 3.1.1 (22-06-08)
# -> Linux 5.10.116 + Patches
SRCREV = "572671db7c996db92ad5d7495e8ec91f76ffc302"
SRC_URI = "git://${KTN_GIT_SERVER}/sw/misc/linux.git;protocol=https;branch=develop-v${LINUX_VERSION}-stm32mp-ktn"

LINUX_TARGET = "stm32mp"
LINUX_RELEASE = "r2.1"

SRC_URI += "file://defconfig"
# Add fragments - required if defconfig shall not be used
#SRC_URI += "file://${LINUX_VERSION}/fragment-03-systemd.config;subdir=fragments"
#SRC_URI += "file://${LINUX_VERSION}/fragment-04-modules.config;subdir=fragments"
#SRC_URI += "file://${LINUX_VERSION}/fragment-05-signature.config;subdir=fragments"

# Use single kernel defconfig file instead of all these kernel config fragments
KERNEL_EXTERNAL_DEFCONFIG = "defconfig"
KERNEL_DEFCONFIG = ""
KERNEL_CONFIG_FRAGMENTS = ""
#KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/fragments/${LINUX_VERSION}/fragment-03-systemd.config"
#KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/fragments/${LINUX_VERSION}/fragment-04-modules.config"
#KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/fragments/${LINUX_VERSION}/fragment-05-signature.config"

# Set CONFIG_LOCALVERSION
LINUX_VERSION_EXTENSION ?= "-ktn"
do_compile:prepend() {
	echo "CONFIG_LOCALVERSION="\"${LINUX_VERSION_EXTENSION}\" >> ${B}/.config
}

S = "${WORKDIR}/git"

do_buildenv[nostamp]="1"
do_buildenv() {
	{
		echo "export MF=\"-j6 ARCH=arm O=${B} CROSS_COMPILE=${CROSS_COMPILE} LOADADDR=${ST_KERNEL_LOADADDR}\""
		echo "export S=${S}"
		echo "export O=${B}"
		echo "export D=${D}"
		echo "alias makey=\"make \${MF}\""
		echo "ln -s \${O} O"
		echo "echo Output directory O=${B}"
		echo "echo Kernel source directory KDIR=${S}"
		echo "echo Makeflags MF=\${MF}"
		echo "echo CROSS_COMPILE=${CROSS_COMPILE}"
		
	} > ${S}/recipe.env
}

addtask buildenv after do_configure before do_devshell
