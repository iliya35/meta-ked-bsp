DESCRIPTION = "A pure Python 2/3 library for peripheral I/O (GPIO, LED, PWM, SPI, I2C, MMIO, Serial) in Linux."
HOMEPAGE = "http://pythonhosted.org/python-periphery/"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://PKG-INFO;md5=b48f82325e5822f1360763ceb2f80420"

PYPI_PACKAGE = "python-periphery"

inherit pypi setuptools3

RDEPENDS:${PN} = "python3-fcntl"

SRC_URI[sha256sum] = "8a8ec019d9b330a6a6f69a7de61d14b4c98b102d76359047c5ce0263e12246a6"
