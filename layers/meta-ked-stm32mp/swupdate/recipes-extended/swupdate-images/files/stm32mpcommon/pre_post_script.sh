#!/bin/sh

# Generate error if ...
set -ue
# var is unset (-u), any command fails (-e)
set -o pipefail
# exit status of pipe is return code of last failed command (-o pipefail)
set -o errtrace
# call traps also for subshells (-E, -o errtrace)

BASEDIR=""
UPDATE_BOOTFS="${BASEDIR}/boot/update"
UPDATE_ROOTFS="${BASEDIR}/mnt/update"

do_preinst() {
	echo "Preparing for update"
	abootctl -vl --begin-update

	echo "Cleaning standby in bootfs (${UPDATE_BOOTFS})"
	rm -rf ${UPDATE_BOOTFS}/*
	
	echo "Cleaning standby rootfs (${UPDATE_ROOTFS})"
	rm -rf ${UPDATE_ROOTFS}/*
	return 0
}

do_postinst() {
	sync
	abootctl -vl --activate-update
	echo "Update is activated! Ready to reboot to finalize update."
	return 0
}

case "$1" in
preinst)
	echo "call do_preinst"
	do_preinst
	;;
postinst)
	echo "call post"
	do_postinst
	;;
esac
