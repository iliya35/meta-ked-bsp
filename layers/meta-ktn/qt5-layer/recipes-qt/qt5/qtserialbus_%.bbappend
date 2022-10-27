DEPENDS = "qtbase"

PACKAGECONFIG ?= "modbus ${@bb.utils.contains('BBFILE_COLLECTIONS', 'openembedded-layer', 'socketcan', '', d)}"
PACKAGECONFIG[modbus] = "-feature-modbus-serialport,-no-feature-modbus-serialport,qtserialport"
PACKAGECONFIG[socketcan] = "-feature-socketcan,-no-feature-socketcan,,libsocketcan"

EXTRA_QMAKEVARS_CONFIGURE += "${PACKAGECONFIG_CONFARGS}"
