SUMMARY = "The minimal Kontron Yocto image"

require recipes-core/images/core-image-minimal.bb
include recipes-core/images/image-ktn-platform.inc
include ${@bb.utils.contains('BBFILE_COLLECTIONS', 'swupdate', 'swupdate/recipes-core/images/image-ktn-swupdate.inc', '', d)}

inherit features_check extrausers userfs

IMAGE_FEATURES ?= ""
IMAGE_LINGUAS = "en-us"

IMAGE_EXTRA_INSTALL ?= ""

IMAGE_INSTALL += " \
        ${@bb.utils.contains('MACHINE_FEATURES', 'wifi-sd8977', 'mrvl-bt8xxx', '', d)} \
        ${@bb.utils.contains('MACHINE_FEATURES', 'wifi-sd8977', 'mrvl-sdio8xxx', '', d)} \
        ${IMAGE_EXTRA_INSTALL} \
        evtest \
        gdbserver \
        i2c-tools \
        libdrm-tests \
        libgpiod-tools \
        mtd-utils \
        nano \
        nfs-utils \
        os-release \
        packagegroup-base \
        production-tool \
        rng-tools \
        u-boot-default-env \
"

# If U-Boot is not included into the rootfs itself, we still need it for
# creating images with wic. Therefore we need a dependency for it.
do_image_wic[depends] += "virtual/bootloader:do_deploy"

do_deploy[vardepsexclude] = "DATETIME"

# if debug-tweaks is unset, set a root password (default is 'root')
ROOT_PWD ?= "\$1\$/nrGReFp\$Emef9p70yRoahjHctKZ2p0"
EXTRA_USERS_PARAMS = "${@bb.utils.contains('IMAGE_FEATURES', 'debug-tweaks', '', 'usermod -p \'${ROOT_PWD}\' root;', d)}"

# reserve extra space of 10 MiB in rootfs image
IMAGE_ROOTFS_EXTRA_SPACE = "40960"
