# This is a class to copy configuration files to an image
# 
# This class shall be a simple solution for cases when images shall be generated
# where only the configuration differs. With this class it is possible to build
# a standard image with special configuration. For example images with different
# hostnames and ip configurations.
#
# Files can be added in different ways:
#
# 1. Add a directory to the SRC_URI containing the configfiles in their target
#    directory tree. If the directory is called "configuration" the contents
#    will be installed to the rootfs automatically. To alter the source dir, set
#    IMAGE_CONFIGFILES_SOURCE accordingly. To change directory permissions or do
#    other manipulations, override or append the do_install_conffiles() task.
#
#    Example:
#
#    SRC_URI = "file://configuration/"
#
# 2. Add single files to the SRC_URI and implement do_install_conffiles() to copy
#    them to the directory IMAGE_CONFIGFILES_INSTALL.
#
#    Example:
#
#    SRC_URI = "file://configuration/interfaces1 file://configuration/conffile2"
#
#    do_install_conffiles() {
#        install -D ${IMAGE_CONFIGFILES_SOURCE}/interfaces1 ${IMAGE_CONFIGFILES_INSTALL}/etc/network/interfaces
#        install -D ${IMAGE_CONFIGFILES_SOURCE}/conffile2 ${IMAGE_CONFIGFILES_INSTALL}/etc/default/conffile
#    }
#
# 3. Add single files to the SRC_URI and use the subdir parameter to install them
#    directly in the install directory.
#
#    Example:
#
#    SRC_URI = " \
#        file://interfaces;subdir=${IMAGE_CONFIGFILES_INSTALL}/etc/network/  \
#        file://conffile;subdir=${IMAGE_CONFIGFILES_INSTALL}/etc/default/   \
#    "
#
# Attention: In all cases files already available in the rootfs are overwritten.
#            The permissions of the original files are lost!

IMAGE_CONFIGFILES_SOURCE ?= "${WORKDIR}/configuration"
IMAGE_CONFIGFILES_INSTALL = "${WORKDIR}/conf_install"

do_clean_conffiles() {
	if ! test -d ${IMAGE_CONFIGFILES_INSTALL}; then
		mkdir ${IMAGE_CONFIGFILES_INSTALL}
	elif test -n "$(ls -1 ${IMAGE_CONFIGFILES_INSTALL})"; then
		rm -rf ${IMAGE_CONFIGFILES_INSTALL}/*
	fi
}
addtask clean_conffiles after do_fetch before do_unpack

do_install_conffiles() {
	if test -d ${IMAGE_CONFIGFILES_SOURCE}; then
		cp -a ${IMAGE_CONFIGFILES_SOURCE}/* ${IMAGE_CONFIGFILES_INSTALL}
	fi
}
addtask install_conffiles after do_unpack before do_rootfs

do_copy_conffiles() {
	if test -n "$(ls -1 ${IMAGE_CONFIGFILES_INSTALL})"; then
		cp -a ${IMAGE_CONFIGFILES_INSTALL}/* ${IMAGE_ROOTFS}
	else
		bbnote "No configfiles installed in ${IMAGE_CONFIGFILES_INSTALL}"
	fi
}
addtask copy_conffiles after do_rootfs before do_image

# The image.bbclass disables fetch and unpack tasks, but we want to use
# them to include configfiles via SRC_URI. Reenable them here.
python __anonymous() {
    d.delVarFlag("do_fetch", "noexec")
    d.delVarFlag("do_unpack", "noexec")
}
