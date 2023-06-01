# Provides the common settings for Kontron i.MX HW

include conf/machine/include/imx-base.inc

PREFERRED_PROVIDER_virtual/kernel ?= "linux-ktn"
PREFERRED_VERSION_linux-ktn ?= "6.1"

PREFERRED_PROVIDER_u-boot ?= "u-boot-ktn"
PREFERRED_PROVIDER_virtual/bootloader ?= "u-boot-ktn"
PREFERRED_PROVIDER_u-boot-fw-utils ?= "u-boot-fw-utils"
PREFERRED_PROVIDER_u-boot-tools ?= "u-boot-imx-tools"
PREFERRED_VERSION_u-boot-ktn ?= "2023.04"
PREFERRED_VERSION_u-boot-fw-utils ?= "2021.07"

PREFERRED_PROVIDER_virtual/trusted-firmware-a ?= "imx-atf"

IMAGE_BOOT_FILES = " \
    fitImage \
"

UBOOT_CONFIG = "${MACHINE}"

UBOOT_INITIAL_ENV = "u-boot-initial-env"

SOC_DEFAULT_IMAGE_FSTYPES = "wic.gz wic.bmap tar.gz"

KERNEL_CLASSES ?= " kernel-fitimage "
KERNEL_IMAGETYPES ?= "fitImage"

# only set features that are common to all machines
MACHINE_FEATURES = "usbhost vfat serial ext2"
