#
# This recipe splits different directories of the root filesystem into separate tar achives.
# Normally it is used to separate the /usr/local contents into a tar achrive to populate the
# user data directory. It is also used to move the contents of the /boot directory into the
# tar achive. 
# All content moved into the tar archives is removed from the rootfs. The base directory is
# kept to provide a mount point for the appropriate partition.
#
# Images using this class should pull in the mount-userfs-partition for their root filesystem.
# If this package is included all mentionned partitions in IMAGE_USERFS(_MOUNT)_LIST are
# automatically mounted on boot.
#

IMAGE_CREATE_USERFS ?= "1"
IMAGE_USERFS_NAME ?= "${IMAGE_BASENAME}-${IMAGE_USERFS_PARTITION_NAME}-${MACHINE}-${DISTRO_CODENAME}"

# userfs working directory
IMAGE_USERFS = "${WORKDIR}/userfs-class/"

# name of mount configuration file
IMAGE_USERFS_MOUNT_CONF = "mount-userfs-partition.conf"

# the name for the userfs partition
IMAGE_USERFS_PARTITION_NAME ?= "userfs"

# the rootfs directory/mountpoint for the userfs
IMAGE_USERFS_DIR ?= "/usr/local"

# create userfs if it not already exists
IMAGE_USERFS_CREATE ?= "1"

# Normally let this list empty. With an empty value it is automatically generated out of
# IMAGE_USERFS_LIST. In very rare cases when the directory and mount point differs, this
# list can be adapted to your needs.
IMAGE_USERFS_MOUNT_LIST ?= ""

# list of all configurations of IMAGE_DIR:IMAGE_PARTITION_NAME:IMAGE_NAME
IMAGE_USERFS_LIST ?= "${IMAGE_USERFS_PARTITION_NAME}:${IMAGE_USERFS_DIR}:${IMAGE_USERFS_NAME}"
