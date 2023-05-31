#!/bin/sh

do_preinst() {
	echo "preinstall\n"
	echo "dummy\n"
	return 0
}

do_postinst() {
	echo "postinstall\n"
	echo "dummy\n"

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
