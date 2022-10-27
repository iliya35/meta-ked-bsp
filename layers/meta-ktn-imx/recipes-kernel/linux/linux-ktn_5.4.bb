require ${PN}-common.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

SRC_URI += "file://patches/kontron-mx6ull-som-expose-only-one-mtd-nand-partition.patch"

# Linux v5.4.105 + KTN patches
SRCBRANCH = "v5.4-ktn"
SRCREV = "90bc1bf84fd550cf2f75d04f2d634a55b021c126"
