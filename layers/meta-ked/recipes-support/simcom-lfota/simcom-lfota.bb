DESCRIPTION = "A small Python CLI tool to update the firmware of SIMCom LTE modules via UART"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=f0ef55c0271bf2dd76834e4371f3e3c7"

SRC_URI = "git://github.com/fschrempf/simcom-lfota.git;branch=main;protocol=https"

SRCREV = "8e55865566fe7d689c94b0b52f2d11b36ba6a30d"

S = "${WORKDIR}/git"

RDEPENDS:${PN} = "python3-core python3-pyserial"

do_install:append() {
	install -d ${D}${bindir}
	install -m 0755 lfota.py ${D}${bindir}
}

FILES:${PN} = "${bindir}/*"
