DESCRIPTION = "Tools to populate test sd card with scripts and firmware"
HOMEPAGE = "https://git.kontron-electronics.de/exceet-apps/setup-prodsd"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=6170f7442efbec5d73a49caedc7607fe"

S = "${WORKDIR}/git"

SRC_URI = "git://${KED_GIT_SERVER_SSH}/exceet-apps/setup-prodsd.git;protocol=ssh;branch=${SRCBRANCH}"
SRCREV = "b359df56c58b7a05e8c42c2c47fcf385396062d7"
SRCBRANCH = "master"

inherit setuptools3

DEPENDS = "python3"
RDEPENDS:${PN} = "\
	python3-core \
	"
