DESCRIPTION = "A Python module to interface addressable WS2812 LEDs via the SPIdev driver in Linux"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "git://github.com/fschrempf/py-neopixel-spidev.git;protocol=https"

SRCREV = "3418307624340fe2ef39bd1cc79c78264848f022"

S = "${WORKDIR}/git"

inherit setuptools3

RDEPENDS:${PN} = "python3-spidev python3-numpy"
RDEPENDS:${PN}-examples = "python3-core python3-image ${PN}"

PACKAGES += "${PN}-examples"

do_install:append() {
	install -d ${D}${datadir}/${PN}/examples
	install -m 0755 wheel.py ${D}${datadir}/${PN}/examples
	install -m 0755 wheel_hsv.py ${D}${datadir}/${PN}/examples
}

FILES:${PN} = "${libdir}/*"
FILES:${PN}-examples = "${datadir}/${PN}/examples/*"
