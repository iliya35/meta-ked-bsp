FILESEXTRAPATHS:append := "${THISDIR}/files:"

PACKAGECONFIG_CONFARGS = ""

SWUPDATE_EXTRA_ARGS = ""

DEPENDS += "zeromq openssl json-c libarchive curl lua mtd-utils libubootenv"

SRC_URI += " \
    file://0001-libubootenv-support-newer-yaml-config.patch \
    file://0002-Always-link-libubootenv.patch \
    file://09-swupdate-args \
    file://default_hwrevision \
    file://default_sw-versions \
    file://default_swupdate.cfg \
    file://fragment_webserver.cfg \
    file://locale-fix.sh \
"

do_install:append() {
    install -d ${D}${sysconfdir}
    install -d ${D}${sysconfdir}/profile.d
    install -m 644 ${WORKDIR}/default_swupdate.cfg ${D}${sysconfdir}/swupdate.cfg
    install -m 644 ${WORKDIR}/default_hwrevision   ${D}${sysconfdir}/hwrevision
    install -m 644 ${WORKDIR}/default_sw-versions   ${D}${sysconfdir}/sw-versions
    install -m 0766 ${WORKDIR}/locale-fix.sh ${D}${sysconfdir}/profile.d/locale-fix.sh
    install -m 0644 ${WORKDIR}/09-swupdate-args ${D}${libdir}/swupdate/conf.d/
}
