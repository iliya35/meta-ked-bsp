DESCRIPTION = "Serial test application"
HOMEPAGE = "https://github.com/cbrake/linux-serial-test"
SECTION = "utils"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSES/MIT;md5=544799d0b492f119fa04641d1b8868ed"

SRC_URI = "git://github.com/cbrake/linux-serial-test.git;protocol=https;branch=master"
SRC_URI[sha256sum] = "1afb7f3b0a9a7a2173cd839cfae7ff92474ce61acfdc860f9fdc14df3c684c8b"
SRCREV = "f8411775c8f9c6f0d1fdd8446841b17950265e92"

BBCLASSEXTEND="native nativesdk"

S = "${WORKDIR}/git"

inherit cmake