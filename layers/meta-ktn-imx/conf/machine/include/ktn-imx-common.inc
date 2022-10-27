# Provides the common settings for Kontron i.MX HW

include conf/machine/include/imx-base.inc

# We want to keep the SOC overrides even for the mainline BSP instead of
# removing them as in meta-freescale. Therefore we only remove the feature
# overrides, so the graphic stack (mesa, etc.) works as intended in
# meta-freescale.
MACHINEOVERRIDES_EXTENDER_FILTER_OUT_use-mainline-bsp = " \
    imxdrm \
    imxdpu \
    imxpxp \
    imxgpu \
    imxgpu2d \
    imxgpu3d \
    imxvpu \
    imxfbdev \
    imxipu \
    "

PREFERRED_PROVIDER_virtual/kernel ?= "linux-ktn"
PREFERRED_PROVIDER_virtual/kernel_realtime ?= "linux-ktn-rt"
PREFERRED_VERSION_linux-ktn ?= "5.10"
PREFERRED_VERSION_linux-ktn-rt ?= "5.4"

PREFERRED_PROVIDER_u-boot ?= "u-boot-ktn"
PREFERRED_PROVIDER_virtual/bootloader ?= "u-boot-ktn"
PREFERRED_PROVIDER_u-boot-fw-utils ?= "u-boot-fw-utils"
PREFERRED_PROVIDER_u-boot-tools ?= "u-boot-imx-tools"
PREFERRED_VERSION_u-boot-ktn ?= "2022.10"
PREFERRED_VERSION_u-boot-fw-utils ?= "2021.07"

PREFERRED_PROVIDER_virtual/trusted-firmware-a ?= "imx-atf"

PREFERRED_PROVIDER_jpeg ?= "jpeg"
PREFERRED_PROVIDER_jpeg-native ?= "jpeg-native"

IMAGE_BOOT_FILES = " \
    fitImage \
"

UBOOT_CONFIG = "${MACHINE}"

UBOOT_ENTRYPOINT = "${UBOOT_LOADADDRESS}"
UBOOT_INITIAL_ENV = "u-boot-initial-env"

#UBOOT_EXTLINUX ?= "1"
#UBOOT_EXTLINUX_LABELS ?= "default"
#UBOOT_EXTLINUX_KERNEL_IMAGE ?= "../fitImage"
#UBOOT_EXTLINUX_FDTDIR ?= ""
#UBOOT_EXTLINUX_KERNEL_ARGS ?= "rootwait"
#UBOOT_EXTLINUX_TIMEOUT = "8"
SOC_DEFAULT_IMAGE_FSTYPES = "wic.gz wic.bmap tar.gz"

KERNEL_CLASSES ?= " kernel-fitimage "
KERNEL_IMAGETYPES ?= "fitImage"

# only set features that are common to all ktn machines
MACHINE_FEATURES = "usbhost vfat serial ext2"
