# OpenST v2.4-stm32mp-r2.1 + KTN Patches
# Based on OpenST 3.1.1 (22-06-08)
# -> trusted-firmaware-a 2.4 + Patches

TF_A_VERSION = "v2.4"
TF_A_SUBVERSION = "stm32mp"
TF_A_RELEASE = "r2.1-ktn"

SRCREV = "b64160cf32352fbdfd91b7f35340d33e781f805d"
SRC_URI = "git://${KTN_GIT_SERVER}/sw/misc/arm-trusted-firmware.git;protocol=https;branch=develop-${TF_A_VERSION}-stm32mp-ktn"
