
# abboot.bbclass
#
# This class modifies the /boot directory to enable an AB boot scheme:
#
# The boot scheme is represented by the following structure:
#
# - Directories 'boot_A' or 'boot_B' for all contents to boot from root filesystem 'A'
#   or root filesystem 'B'. For example the Linux kernel and device trees or the fit
#   image is common content for these directories
#
# - The files 'boot_A/.is_A' and 'boot_B/.is_B' to indicate the directories
#   corresponding root filesystem 'A' or 'B'. This is useful to ease a bootloader
#   implementation.
#
# - Symbolic links 'active' and 'standby' which point to the currently activated
#   root filesystem (active Link) or the 'spare' root filesystem for rescue or
#   for the space of the next update (standby Link). These links point to either
#   'boot_A' or 'boot_B' directory.
#
# This recipe generates two tar archives:
#
# - The 'BOOTFILES_ARCHIVE' which contains all files found in the /boot directory
#   of the root filesystem. This archive is useful to include in update packages
#   to update the 'boot_A' or 'boot_B' directories on the target system when
#   updating.
#
# - The 'BOOTFS_NAME' is the processed content of the /boot directory of the
#   root filesystem. All boot files are distributed to 'root_A' and 'root_B'
#   directories and the active and standby link is set to 'root_A' and 'root_B'
#
# This class inherits the userfs class to divide the root filesystem into several
# pieces. Because of this a 'userfs' from /usr/local is also generated if
# 'IMAGE_USERFS_NAME' is not cleared.
#
inherit userfs-settings

IMAGE_BOOTFS_NAME ?= "${IMAGE_BASENAME}-bootfs-${MACHINE}-${DISTRO_CODENAME}"
IMAGE_BOOTFILES_ARCHIVE ?= "${IMAGE_BASENAME}-bootfiles-${MACHINE}-${DISTRO_CODENAME}"

IMAGE_USERFS_LIST:append = " boot:/boot:${IMAGE_BOOTFS_NAME}"

fakeroot do_abboot() {
	# remove old files
	rm -rf ${IMAGE_ROOTFS}/boot/root_A ${IMAGE_ROOTFS}/boot/root_B
	rm -rf ${IMAGE_ROOTFS}/boot/active ${IMAGE_ROOTFS}/boot/standby

	(
		cd ${IMAGE_ROOTFS}/boot
		tar czf ${IMGDEPLOYDIR}/${IMAGE_BOOTFILES_ARCHIVE}.tar.gz ./*
	)

	# create root_A (active)
	mkdir ${IMAGE_ROOTFS}/boot/root_A
	ln -s root_A ${IMAGE_ROOTFS}/boot/active
	find ${IMAGE_ROOTFS}/boot -maxdepth 1 -type f -exec mv {} ${IMAGE_ROOTFS}/boot/root_A \;

	# move extlinux for compatibility
	find ${IMAGE_ROOTFS}/boot -maxdepth 1 -type d -name "*_extlinux" -exec mv {} ${IMAGE_ROOTFS}/boot/root_A \;
	
	# create root_B (standby)
	cp -a ${IMAGE_ROOTFS}/boot/root_A ${IMAGE_ROOTFS}/boot/root_B
	ln -s root_B ${IMAGE_ROOTFS}/boot/standby

	# identify filesystems for bootloader
	touch ${IMAGE_ROOTFS}/boot/root_A/.is_A
	touch ${IMAGE_ROOTFS}/boot/root_B/.is_B

	# create boot link for compatibility
	rm -f ${IMAGE_ROOTFS}/boot/boot
	ln -s root_A ${IMAGE_ROOTFS}/boot/boot
}

addtask abboot after do_rootfs before do_image_qa do_userfs
userfs[depends] += "virtual/fakeroot-native:do_populate_sysroot"
