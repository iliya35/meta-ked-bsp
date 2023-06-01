require recipes-core/images/image-ked.bb

IMAGE_FEATURES += "debug-tweaks"

inherit image-configuration

SRC_URI += "file://configuration/"

IMAGE_INSTALL += " \
	${@bb.utils.contains('MACHINE_FEATURES', 'screen', 'touchtest-emv', '', d)} \
	sparkle-autostart \
	hostapd \
	dnsmasq \
"
