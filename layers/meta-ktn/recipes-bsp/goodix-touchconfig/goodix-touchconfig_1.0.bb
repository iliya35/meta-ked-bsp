DESCRIPTION = "Generates configuration file for Goodix TS and puts it in rootfs for driver to load"

LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "file://goodix_911_cfg"
SRC_URI += "file://goodix_911_50inch.cfg"
SRC_URI += "file://touch-goodix-generate-fw.sh"

S="${WORKDIR}"

# Default Values
# can be overridden in bbappends
GOODIX_TOUCHPOINTS ?= "5"
GOODIX_CONFIGFILE ?= "goodix_911_cfg"

do_compile() {
	# Replace values in textfile
	# add more lines like these for other parameters
	sed -i 's/./${GOODIX_TOUCHPOINTS}/17' ${S}/${GOODIX_CONFIGFILE}

	# Generate binary file from text file
	if ! type xxd > /dev/null 2>&1; then
		echo "ERROR: xxd is required to compile goodix-touconfig";
		exit 1
	fi
	${S}/touch-goodix-generate-fw.sh ${S}/${GOODIX_CONFIGFILE}
	
}

do_install() {
	install -D ${S}/${GOODIX_CONFIGFILE}.bin ${D}/lib/firmware/goodix_911_cfg.bin
}

FILES:${PN} = "/lib/firmware/goodix_911_cfg.bin"
FILES:${PN}-dbg = ""
