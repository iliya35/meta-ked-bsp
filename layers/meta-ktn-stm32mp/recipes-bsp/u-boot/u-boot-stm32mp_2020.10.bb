# OpenST v2020.10-stm32mp-r2.1 + KTN Patches
# Based on OpenST 3.1.1 (22-06-08)
# -> u-boot 2020.10 + Patches

require u-boot-stm32mp-common_${PV}.inc
require recipes-bsp/u-boot/u-boot-stm32mp.inc

SUMMARY = "Universal Boot Loader for embedded devices for stm32mp"
LICENSE = "GPL-2.0-or-later"

PROVIDES += "u-boot"
RPROVIDES:${PN} += "u-boot"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'u-boot-stm32mp-archiver.inc','')}
