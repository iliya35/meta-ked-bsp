DESCRIPTION = "ptool is used to run testing, production and serialization tasks on Kontron Linux hardware"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit update-rc.d

PV = "0.4+git${SRCPV}"

SRCBRANCH = "develop"

SRCREV = "d4aa99622c79db2862b27e83d1b61da36698c186"

#SRCREV = "${AUTOREV}"
SRC_URI = " \
    git://${KTN_GIT_APPS}/ptool.git;protocol=https;branch=${SRCBRANCH}    \
    file://prod-env.sh;subdir=git                                       \
    file://production-prod.sh;subdir=git                                \
"

PACKAGES += "${PN}-prod ${PN}-autoprod"

FILES:${PN} = "/usr/bin/ptool /usr/bin/production-tool.sh /usr/share/production/productionLib.sh /usr/share/production/prod-env.sh /usr/bin/fwinfo"
FILES:${PN}-prod = "/usr/share/production/testLib.sh /usr/share/production/production-prod.sh /usr/share/production/resizesd.sh"
FILES:${PN}-autoprod = "/etc/init.d/autoprod"
FILES:${PN}-dbg = ""

RDEPENDS:${PN}-autoprod = "${PN}-prod"
RDEPENDS:${PN}-prod = "bash"
RDEPENDS:${PN} = "ethtool u-boot-fw-utils u-boot-tools-mkenvimage e2fsprogs e2fsprogs-resize2fs ntpdate"

INITSCRIPT_PACKAGES = "${PN}-autoprod"
INITSCRIPT_NAME:${PN}-autoprod = "autoprod"
INITSCRIPT_PARAMS:${PN}-autoprod = "start 99 5 ."

S = "${WORKDIR}/git"

do_install () {
    install -D -m 0755 ${S}/ptool			${D}/usr/bin/ptool
    install -D -m 0755 ${S}/fwinfo			${D}/usr/bin/fwinfo
    install -D -m 0755 ${S}/production-prod.sh		${D}/usr/share/production/production-prod.sh
    install -D -m 0755 ${S}/testLib.sh			${D}/usr/share/production/testLib.sh
    install -D -m 0755 ${S}/productionLib.sh		${D}/usr/share/production/productionLib.sh
    install -D -m 0755 ${S}/resizesd.sh		${D}/usr/share/production/resizesd.sh
    install -D -m 0755 ${S}/prod-env.sh		${D}/usr/share/production/prod-env.sh
    install -d ${D}${sysconfdir}/init.d
    install -D -m 0755 ${S}/autoprod.sh			${D}${sysconfdir}/init.d/autoprod

    echo "echo '###################################################\n\
Note: production-tool.sh was renamed to ptool.\n\
Please use ptool in the future!\n\
###################################################'; ptool \$@;" > ${D}/usr/bin/production-tool.sh

    chmod +x ${D}/usr/bin/production-tool.sh
}
