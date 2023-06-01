SUMMARY = "The basic Kontron Yocto image"

require recipes-core/images/image-ked-minimal.bb

IMAGE_FEATURES += "package-management ssh-server-openssh splash"

IMAGE_EXTRA_INSTALL += "openssh-sftp-server \
			udev-extraconf \
		       "
