include conf/machine/include/tune-cortexa53.inc
include conf/machine/include/arm/armv8a/tune-cortexa53.inc

MACHINEOVERRIDES =. "mx8:mx8m:mx8mm:ktn-mx8mm:"

# Mainline BSP defaults to "generic" cortexa53 configuration,
# adjust it here to include crypto extension which enables
# inline NEON and FPU code generation
DEFAULTTUNE:use-mainline-bsp = "cortexa53-crypto"

UBOOT_BINARY ?= "flash.bin"
UBOOT_SUFFIX ?= "bin"

TFA_PLATFORM = "imx8mm"
TFA_BINARY = "bl31-imx8mm.bin"
TFA_BUILD_TARGET = "bl31"
DDR_FIRMWARE_NAME = "lpddr4_pmu_train_1d_imem.bin \
		     lpddr4_pmu_train_1d_dmem.bin \
		     lpddr4_pmu_train_2d_imem.bin \
		     lpddr4_pmu_train_2d_dmem.bin"

UBOOT_EXTLINUX_CONSOLE = "console=ttymxc2,115200"
UBOOT_EXTLINUX_ROOT = "root=/dev/mmcblk1p2"

WKS_FILE = "image-imx-ktn.wks"
IMX_BOOT_SEEK = "33"
WICVARS:append = " IMX_BOOT_SEEK UBOOT_BINARY"
WKS_FILE_DEPENDS:remove = "imx-boot"

UBOOT_ENTRYPOINT:mx8mm:remove = "0x40480000"
UBOOT_ENTRYPOINT:ktn-mx8mm:append = "0x40400000"
