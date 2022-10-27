#!/bin/sh

# Set defaults
ENABLE=1
EXTRA_ARGS=
LOGFILE=/var/log/ktnstressd

if test -f /etc/default/ktnstressd; then
    . /etc/default/ktnstressd
fi

if test "${ENABLE}" -eq 1; then
    ( /usr/bin/ktnstress.sh ${EXTRA_ARGS} 2>&1 | tee ${LOGFILE} )&
fi
