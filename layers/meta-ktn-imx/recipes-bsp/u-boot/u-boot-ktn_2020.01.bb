require u-boot-ktn-common.inc
require recipes-bsp/u-boot/u-boot.inc

DEPENDS += "bc-native dtc-native u-boot-mkenvimage-native"

# v2020.01 + KTN Patches
SRCREV = "78e644d54f0b5fc14879023263e3b7246ee00af8"
SRCBRANCH = "develop-v2020.01-ktn"

SRC_URI[md5sum] = "d86276b74616cc9945e38fc1412772c9"
SRC_URI[sha256sum] = "7a986967fd3a79432a9a0a6a3ff27edb67c6e96583cb4abeb886fdd7f74f0824"

SRC_URI:append_ktn-mx6ul = " \
    file://fw_env.config \
    file://uboot-env-mx6ul.txt; \
"

inherit fsl-u-boot-localversion

# backported upstream changes
inherit python3native
DEPENDS:remove = "python-native"
EXTRA_OEMAKE += 'STAGING_INCDIR=${STAGING_INCDIR_NATIVE} STAGING_LIBDIR=${STAGING_LIBDIR_NATIVE}'

SCMVERSION = "y"
LOCALVERSION = "_${DISTRO_CODENAME}_${DISTRO_VERSION}"

# Set custom make targets for SPL and U-Boot proper
UBOOT_MAKE_TARGET_mx8 = "flash.bin u-boot.itb"

# deploy uboot-env binary
do_deploy:append() {
    if [ -n "${UBOOT_ENV}" ]; then
        mkenvimage -r -s 0x10000 -o ${DEPLOYDIR}/${UBOOT_ENV}.bin ${DEPLOYDIR}/${UBOOT_ENV_BINARY}
    fi
}
