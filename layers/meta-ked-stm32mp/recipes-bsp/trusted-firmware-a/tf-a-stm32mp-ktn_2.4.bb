require recipes-bsp/trusted-firmware-a/tf-a-stm32mp-common.inc
include recipes-bsp/trusted-firmware-a/tf-a-stm32mp.inc

SUMMARY = "Trusted Firmware-A for STM32MP1"
SECTION = "bootloaders"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

PROVIDES += "virtual/trusted-firmware-a"

SRCREV = "6bbbf4489d8c0935da9b7363dd0059703c590e2f"
SRC_URI = "git://${KED_GIT_SERVER}/sw/misc/arm-trusted-firmware.git;protocol=https;branch=develop-${TF_A_VERSION}-stm32mp-ktn"

TF_A_VERSION = "v2.4"
TF_A_SUBVERSION = "stm32mp"
TF_A_RELEASE = "r2.1-ktn"
PV = "${TF_A_VERSION}-${TF_A_SUBVERSION}-${TF_A_RELEASE}"

ARCHIVER_ST_BRANCH = "${TF_A_VERSION}-${TF_A_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "master"
ARCHIVER_COMMUNITY_REVISION = "${TF_A_VERSION}"

S = "${WORKDIR}/git"

# Configure settings
TFA_PLATFORM  = "stm32mp1"
TFA_ARM_MAJOR = "7"
TFA_ARM_ARCH  = "aarch32"

# Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ?= "1"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-a-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/arm-trusted-firmware.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV_class-devupstream = "a47302b7b05a9c1e27f62b08fe8f66ca422ef174"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
