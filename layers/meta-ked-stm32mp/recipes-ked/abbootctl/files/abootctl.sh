#!/bin/sh

DISABLED=0

ABOOTCTL_PROGRAM="/usr/bin/abootctl"
PROGRAM_ARGS="-vr"

UPDATE_MOTD=1

config_path="/etc/default/abootctl"


start_program()
{
    echo "BOOTSTATE:"
    ${ABOOTCTL_PROGRAM} ${PROGRAM_ARGS}
}


if test -f $config_path;then
	. $config_path
fi

if test "${DISABLED}" == "1"; then
    exit 0
fi

if test "${UPDATE_MOTD}" == "1"; then
    start_program | tee /etc/motd
    cat /etc/motd > /dev/console
else
    start_program
fi
