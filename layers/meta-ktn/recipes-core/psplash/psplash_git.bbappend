FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SPLASH_IMAGES = "file://kontronelectronics_small.png;outsuffix=default"

FILES:${PN}:append = " /mnt/.psplash/dummy"

do_install:append() {
    install -d ${D}/mnt/.psplash
    touch ${D}/mnt/.psplash/dummy
}
