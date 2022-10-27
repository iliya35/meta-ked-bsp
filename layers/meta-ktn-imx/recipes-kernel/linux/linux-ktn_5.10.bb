require ${PN}-common.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

# Fix missing gmp.h issues on kernel >= 5.8
DEPENDS += "gmp-native"
EXTRA_OEMAKE += "HOSTCXX="${BUILD_CXX} ${BUILD_CXXFLAGS} ${BUILD_LDFLAGS}""

# Linux v5.10.149 + KTN patches
SRCBRANCH = "v5.10-ktn"
SRCREV = "1484564207cce797a6b0f6fc85e7281944ba683b"
