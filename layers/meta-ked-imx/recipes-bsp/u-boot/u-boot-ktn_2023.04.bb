require u-boot-ktn-common.inc
require recipes-bsp/u-boot/u-boot.inc

LIC_FILES_CHKSUM = "file://Licenses/README;md5=2ca5f2c35c8cc335f0a19756634782f1"

DEPENDS += "bc-native dtc-native gnutls-native"

# v2023.04 + KED Patches
SRCREV = "a15acbb0b04a30ced011c330838e2b7d058912c5"
SRCBRANCH = "develop-v2023.04-ktn-imx"

SRC_URI[md5sum] = "d86276b74616cc9945e38fc1412772c9"
SRC_URI[sha256sum] = "7a986967fd3a79432a9a0a6a3ff27edb67c6e96583cb4abeb886fdd7f74f0824"

SRC_URI:append = " \
    file://fw_env.config \
"

# Overwrite the default environment
SRC_URI:append:ked-mx8mp = " \
    file://osm-s-mx8mp.env;subdir=git/board/kontron/osm-s-mx8mp/ \
"

SRC_URI:append:ked-mx8mm = " \
    file://sl-mx8mm.env;subdir=git/board/kontron/sl-mx8mm/ \
"

SRC_URI:append:ked-mx6ul = " \
    file://sl-mx6ul.env;subdir=git/board/kontron/sl-mx6ul/ \
"

inherit fsl-u-boot-localversion

# backported upstream changes
inherit python3native
DEPENDS:remove = "python-native"
EXTRA_OEMAKE += 'STAGING_INCDIR=${STAGING_INCDIR_NATIVE} STAGING_LIBDIR=${STAGING_LIBDIR_NATIVE}'

SCMVERSION = "y"
LOCALVERSION = "_${DISTRO_CODENAME}_${DISTRO_VERSION}"
