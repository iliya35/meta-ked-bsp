include conf/machine/include/tune-cortexa53.inc
include conf/machine/include/arm/armv8a/tune-cortexa53.inc

MACHINEOVERRIDES =. "ked-mx8mp:ked-mx8m:ked-imx:mx8mp:mx8m:"

# Mainline BSP defaults to "generic" cortexa53 configuration,
# adjust it here to include crypto extension which enables
# inline NEON and FPU code generation
DEFAULTTUNE:use-mainline-bsp = "cortexa53-crypto"

UBOOT_BINARY ?= "flash.bin"
UBOOT_SUFFIX ?= "bin"
UBOOT_MAKE_TARGET ?= "flash.bin"

TFA_PLATFORM = "imx8mp"
TFA_BINARY = "bl31-imx8mp.bin"
TFA_BUILD_TARGET = "bl31"
ATF_BOOT_UART_BASE = "0x30880000"
DDR_FIRMWARE_NAME = "lpddr4_pmu_train_1d_imem_202006.bin \
		     lpddr4_pmu_train_1d_dmem_202006.bin \
		     lpddr4_pmu_train_2d_imem_202006.bin \
		     lpddr4_pmu_train_2d_dmem_202006.bin"

WKS_FILE = "image-imx-ked.wks"
IMX_BOOT_SEEK = "32"
WICVARS:append = " IMX_BOOT_SEEK UBOOT_BINARY"
WKS_FILE_DEPENDS:remove = "imx-boot"

UBOOT_ENTRYPOINT = "0x40400000"
UBOOT_ENTRYPOINT:mx8m-generic-bsp = "0x40400000"

UBOOT_DTB_LOADADDRESS = "0x48000000"
UBOOT_DTBO_LOADADDRESS = "0x49000000"
