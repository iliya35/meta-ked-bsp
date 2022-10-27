#@TYPE: Platform template
#@NAME: stm32mp-t1000-common
#@DESCRIPTION: Common machine configuration for t1000 hardware platform
#@NEEDED_BSPLAYERS: meta-st-stm32mp

# =========================================================================
# Default providers linux, u-boot, tf-a, optee
# =========================================================================
PREFERRED_PROVIDER_virtual/kernel ?= "linux-stm32mp"
#PREFERRED_VERSION_linux-stm32mp ?= "5.10"
PREFERRED_PROVIDER_virtual/bootloader ?= "u-boot-stm32mp"
#PREFERRED_VERSION_u-boot-stm32mp ?= "2020.10"
PREFERRED_PROVIDER_virtual/trusted-firmware-a ?= "tf-a-stm32mp"
PREFERRED_VERSION_trusted-firmware-a ?= "2.4"

include conf/machine/include/st-machine-common-stm32mp.inc
# =========================================================================
# Remove obsolete settings from ST
# =========================================================================
MACHINE_EXTRA_RRECOMMENDS:remove = " linux-examples-stm32mp1 "
MACHINE_EXTRA_RRECOMMENDS:remove = " m4projects-stm32mp1 "
INHERIT:remove = " flashlayout-stm32mp "
EXTRA_IMAGEDEPENDS:remove = "sdcard-raw-tools-native"
IMAGE_CLASSES:remove = "image_types-stubi st-partitions-image"

# If required extra partitions are defined in main rootfs image, not in machine
PARTITIONS_IMAGE = ""
PARTITIONS_MOUNTPOINT_IMAGE = ""

# We don't use STs UBI image generation options
ENABLE_MULTIVOLUME_UBI="0"

# Don't generate flash layout
ENABLE_FLASHLAYOUT_CONFIG="0"

# Disable ST's images
ST_BOOTFS   = "0"
ST_VENDORFS = "0"
ST_USERFS   = "0"

# Improve build speed
ENABLE_IMAGE_LICENSE_SUMMARY = "0"

# Currently fib is not supported
MACHINE_FEATURES:remove = "fip"

# =========================================================================
# Machine specific software packages
# =========================================================================

# HACK: Remove imx specific ptool. production-tool is included in image-ktn-minimal by default
IMAGE_INSTALL:remove = "production-tool"

CORE_IMAGE_EXTRA_INSTALL:append += ""

# List of packages required to boot the device
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS += "\
	u-boot-stm32mp-extlinux \
	u-boot-fw-utils \
	kernel-imagebootfs \
	"

# List of packages useful to use device
# Should be probably moved to distro
MACHINE_EXTRA_RRECOMMENDS += "\
	rng-tools \
    ${@bb.utils.contains('DISTRO_FEATURES','sysvinit',' watchdog ','',d)} \
    "

# List of packages useful to use device
# Recommend machine-specific packages to install (Touchscreen drivers)
MACHINE_EXTRA_RRECOMMENDS += "\
    kernel-modules \
	mptool \
	"

# autoload the modules
KERNEL_MODULE_AUTOLOAD += ""

# =========================================================================
# Image
# =========================================================================

# Only build tar.gz
IMAGE_FSTYPES = "tar.gz"



# =========================================================================
# Machine settings
# =========================================================================
# Define specific common layer name
MACHINEOVERRIDES =. "stm32mp-t10xx:"

# Extend machine feature
MACHINE_FEATURES += "${@'gpu' if d.getVar('ACCEPT_EULA_'+d.getVar('MACHINE', True), True) == '1' else ''}"

# Default serial consoles (TTYs) to enable using getty
SERIAL_CONSOLES = "115200;ttySTM0"
SERIAL_CONSOLE = "115200 ttySTM0"

# What should be built alongside this image
EXTRA_IMAGEDEPENDS += "tf-a-stm32mp"
EXTRA_IMAGEDEPENDS += "u-boot-stm32mp"

# Chip architecture
DEFAULTTUNE = "cortexa7thf-neon-vfpv4"
include conf/machine/include/tune-cortexa7.inc

# Kernel settings
ST_KERNEL_LOADADDR   = "0xC2000040"
KERNEL_DEFCONFIG    ?= "stm32mp-t10xx"

# =========================================================================
# Common Extlinux boot settings
# =========================================================================
# Define boot command lines
#
# The following bootschemes are supported
# - boot from sdcard (BOOTPIN_0,2=1, BOOTPIN_1=0)
# - boot from nor (BOOTPIN_0=1, BOOTPIN_3,2=0)
#   When booting from nor u-boot variable 'boot_targets' select where to find
#   the bootfs with EXTLINUX configuration
#

# Add extlinux definintions for boards
UBOOT_EXTLINUX_TIMEOUT = "-1"
UBOOT_EXTLINUX_KERNEL_IMAGE = "/boot/${KERNEL_IMAGETYPE}"
UBOOT_EXTLINUX_CONFIG_FLAGS = "sdcard nor-emmc nor-nand"


# =========================================================================
# Specific features settings
# =========================================================================
# OPTEE
OPTEE_OS_WITH_PAGER = "n"
