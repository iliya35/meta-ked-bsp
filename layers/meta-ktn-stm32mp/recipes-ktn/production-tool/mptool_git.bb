FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:${THISDIR}/${PN}/common:"

DESCRIPTION = "mptool is used to run testing, production and serialization tasks on Linux hardware"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=40342a47ce18cda6e09828180f74ab26"

SRCBRANCH = "master"

RDEPENDS:${PN} += "bash"

SRCREV = "0e994a5100656ad15e330c2b7ab93ede2b3a6087"
SRC_URI = "git://${KTN_GIT_SERVER}/sw/misc/apps/mptool.git;protocol=ssh;branch=${SRCBRANCH}"
SRC_URI_http = "git://${KTN_GIT_SERVER}/sw/misc/apps/mptool.git;protocol=http;branch=${SRCBRANCH}"

SRC_URI:append = "\
	file://mptool.config;subdir=git \
	file://create-sd-card.sh \
	file://sd-card.layout \
	file://image-ktn-stm32mp-t1000.layout \
	file://image-ktn-stm32mp-t1001.layout \
	file://image-ktn-stm32mp-t1002.layout \
	file://image-ktn-stm32mp-t1004.layout \
	file://image-ktn-stm32mp-t1005.layout \
	file://image-ktn-stm32mp-t1006.layout \
	file://image-ktn-stm32mp-t1007.layout \
	file://image-ktn-qt-stm32mp-t1000-multi.layout \
	file://image-ktn-stm32mp-t1000-multi.layout \
	file://image-ktn-sparkle-stm32mp-t1000-multi.layout \
	file://image-ktn-swu-stm32mp-t1000-multi.layout \
	file://image-ktn-qt-swu-stm32mp-t1000-multi.layout \
	file://image-ktn-test-stm32mp-t1000-multi.layout \
	file://image-ktn-stress-stm32mp-t1000-multi.layout \
	"

# Add extra EMMC configuration for Eval-Kits
SRC_URI:append:stm32mp-t1000-multi += "file://mptool_EMMC.config;subdir=git"

FILES:${PN} += "\
	/usr \
	/etc \
	"
S = "${WORKDIR}/git"

do_install () {
	install -Dm 0755 ${S}/mptool ${D}${bindir}/mptool
	install -Dm 0755 ${S}/productionLib.sh ${D}/usr/lib/mptool/productionLib.sh	
	install -Dm 0755 ${S}/mptool-classic ${D}${bindir}/mptool-classic
	install -Dm 0755 ${S}/productionLib-classic.sh ${D}/usr/lib/mptool/productionLib-classic.sh	
	install -d ${D}${sysconfdir}
	install -Dm 0755 ${S}/mptool.config.fullsample ${D}${sysconfdir}/mptool.config.sample
	install -Dm 0755 ${S}/mptool*.config ${D}${sysconfdir}
	install -Dm 0755 ${S}/fwinfo ${D}${bindir}/fwinfo
}

inherit deploy

do_deploy () {
	install -d ${DEPLOYDIR}/script-mp
    install -m 0755 ${WORKDIR}/create-sd-card.sh ${DEPLOYDIR}/script-mp/
    install -m 0755 ${WORKDIR}/sd-card.layout ${DEPLOYDIR}/script-mp/
  
  	bbnote "Searching for files ... ${MACHINE}.layout"
	for layout in $(ls -1 ${WORKDIR}/*-${MACHINE}.layout); do
		bbnote cp ${layout} ${DEPLOYDIR}/script-mp/$(basename ${layout})
		cp ${layout} ${DEPLOYDIR}/script-mp/$(basename ${layout})
	done
}

addtask deploy before do_build after do_compile
