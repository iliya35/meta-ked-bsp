FILESEXTRAPATHS:append := "${THISDIR}/files:"

SRC_URI:append = "file://05-defaults"

do_install:append () {
    install -d ${D}${libdir}/swupdate/conf.d
    install -m 644 ${WORKDIR}/05-defaults ${D}${libdir}/swupdate/conf.d/
}