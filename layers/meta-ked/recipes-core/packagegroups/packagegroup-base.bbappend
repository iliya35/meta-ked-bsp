# Add packages for CAN if hardware supports it
PACKAGES += "packagegroup-base-can"
RDEPENDS:packagegroup-base += "packagegroup-base-can"
SUMMARY:packagegroup-base-can = "CAN support"
RDEPENDS:packagegroup-base-can = "can-utils libsocketcan"

# Add packages for UBIFS if hardware has NAND flash
PACKAGES += "packagegroup-base-ubifs"
RDEPENDS:packagegroup-base += "packagegroup-base-ubifs"
SUMMARY:packagegroup-base-ubifs = "UBIFS support"
RDEPENDS:packagegroup-base-ubifs = "mtd-utils-ubifs"

# Add firmware for WiFi if hardware has sd8787-based WiFi (e.g. uBlox ELLA)
RDEPENDS:packagegroup-base-wifi += "linux-firmware-sd8787"

# Add firmware for WiFi if hardware has sd8977-based WiFi (e.g. Panasonic 9026)
RDEPENDS:packagegroup-base-wifi += "linux-firmware-sd8977"

# Add firmware for USB-WiFi (Ralink) if hardware has usbhost in MACHINE_FEATURES
RDEPENDS:packagegroup-base-wifi += "linux-firmware-ralink"

# Add alsa apps
RRECOMMENDS:packagegroup-base-alsa += " alsa-state alsa-utils alsa-tools"

# Add beeper support for boards with pwm-buzzer
RDEPENDS:packagegroup-base += "beep"
