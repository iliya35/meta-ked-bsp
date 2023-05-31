#!/bin/sh

# Default values
START=1
QML_SOFTWARE_RENDERING=1

# Source settings from config file
if test -f /etc/default/settings-touchtest-emv; then
    source /etc/default/settings-touchtest-emv
fi

if test ${START} = "0"; then
    logger -p user.notice "touchtest-emv not starting due to setting START=${START} in /etc/default/settings-touchtest-emv"
    echo ""
    exit 0
fi

if [ $QML_SOFTWARE_RENDERING -ne 0 ]; then
	export QT_QUICK_BACKEND=software
else
	unset QT_QUICK_BACKEND
fi 


echo "Starting touchtest-emv"
logger -p user.info "touchtest-emv started"

echo 0 > /sys/class/graphics/fbcon/cursor_blink

(
    /usr/bin/touchtest-emv
    logger -p user.info "touchtest-emv finished"
)&

