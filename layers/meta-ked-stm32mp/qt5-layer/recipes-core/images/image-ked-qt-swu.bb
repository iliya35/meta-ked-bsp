SUMMARY = "The Qt5 Kontron Yocto image with SWUpdate support"
# Use the AB boot layout
inherit abboot

require qt5-layer/recipes-core/images/image-ked-qt.bb

# /boot and /usr/local contents are located in different partitions
IMAGE_BOOTFS_NAME = "${IMAGE_BASENAME}-bootfs-${MACHINE}"
IMAGE_BOOTFILES_ARCHIVE = "${IMAGE_BASENAME}-bootfiles-${MACHINE}"
IMAGE_USERFS_NAME = "${IMAGE_BASENAME}-userfs-${MACHINE}"

IMAGE_FEATURES += ""

IMAGE_EXTRA_INSTALL += "abootctl-autostart"

# remove extlinux
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS:remove = "u-boot-stm32mp-extlinux"
