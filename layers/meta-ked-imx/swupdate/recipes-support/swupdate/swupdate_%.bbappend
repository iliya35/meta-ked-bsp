FILESEXTRAPATHS:append := "${THISDIR}/files:"

SRC_URI:append:kontron-mx8mm = " \
    file://diskpart.cfg \
    file://uboot-env.cfg \
"
