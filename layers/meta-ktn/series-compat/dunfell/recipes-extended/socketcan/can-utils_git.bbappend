LICENSE = "GPLv2 & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://include/linux/can.h;endline=44;md5=a9e1169c6c9a114a61329e99f86fdd31"

SRCREV = "3615bac17e539a06835dcb90855eae844ee18053"

PV = "2021.08.0"

S = "${WORKDIR}/git"

inherit update-alternatives

ALTERNATIVE:${PN} = "candump cansend cansequence"
ALTERNATIVE_LINK_NAME[candump] = "${bindir}/candump"
ALTERNATIVE_LINK_NAME[cansend] = "${bindir}/cansend"
ALTERNATIVE_LINK_NAME[cansequence] = "${bindir}/cansequence"

# busybox ip fails to configure can interfaces, so we need iproute2 to do so.
# See details in http://www.armadeus.com/wiki/index.php?title=CAN_bus_Linux_driver.
RRECOMMENDS:${PN} += "iproute2"
