DESCRIPTION = "A demo webapp to provide a simple dynamic website via webserver"
HOMEPAGE = ""
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

RDEPENDS:${PN} = "lighttpd hostapd dnsmasq lighttpd-module-fastcgi lighttpd-module-redirect php-cgi"

SRCBRANCH = "master"
SRCREV = "10661b416ac9fe5df42e36a5f22502402505f82b"
SRC_URI = "git://${KED_GIT_APPS}/web-demo.git;protocol=ssh;branch=${SRCBRANCH}"
SRC_URI += "file://demoserver.conf"

FILES:${PN} = "/var/www ${sysconfdir}/lighttpd.d"

S="${WORKDIR}/git"

do_install () {
    install -d -m 0755 ${D}/var/www
    cp -r ${S}/_site/* ${D}/var/www

    install -d ${D}${sysconfdir}/lighttpd.d/
    install -m 0644 ${WORKDIR}/demoserver.conf ${D}${sysconfdir}/lighttpd.d
}
