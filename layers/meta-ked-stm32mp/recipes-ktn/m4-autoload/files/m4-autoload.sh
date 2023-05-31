#!/bin/sh

#copy firmware name to remoteproc service and start it

PROGRAM="m4-autoload"
DT_APPLICATION_NODE="/proc/device-tree/m4-application"
APPLICATION_DEFAULT="m4-application.elf"
APPLICATION_POOL="/usr/share/m4-application"

rproc_dir="/sys/class/remoteproc/remoteproc0"
source_config="/etc/default/m4-autoload.conf"

# Fetch default application name
if test -f "${DT_APPLICATION_NODE}"; then
	APPLICATION_NAME=$(strings ${DT_APPLICATION_NODE})
else
	APPLICATION_NAME=${APPLICATION_DEFAULT}
fi

if test -f $source_config;then
	. $source_config
fi

do_find_application ()
{
	if test -z "${APPLICATION_NAME}"; then
		echo "No M4 application to load"
		exit 0
	fi

	if test ! -f "${APPLICATION_NAME}"; then
		if test -f "${APPLICATION_POOL}/${APPLICATION_NAME}"; then
			APPLICATION_NAME="${APPLICATION_POOL}/${APPLICATION_NAME}"
		else
			echo "ERROR: M4 application '${APPLICATION_NAME}' not found "
			exit 1
		fi
	fi

	# Generate absolute name
	APPLICATION_NAME_ABSOLUTE=$(realpath ${APPLICATION_NAME})
	APPLICATION_NAME="$(basename ${APPLICATION_NAME_ABSOLUTE})"
	APPLICATION_PATH="$(dirname ${APPLICATION_NAME_ABSOLUTE})"
}

do_start()
{
	echo "Stopping previous application"
	do_stop
	do_find_application

	# Link name and start application
	if test "${APPLICATION_PATH}" != "/lib/firmware"; then
		rm -f "/lib/firmware/${APPLICATION_NAME}"
		ln -s "${APPLICATION_NAME_ABSOLUTE}" "/lib/firmware/${APPLICATION_NAME}"
	fi
	echo -n "${APPLICATION_NAME}" > $rproc_dir/firmware
	echo -n start > $rproc_dir/state
	for i in 1 2 3 4 5 6 7 8 9 10; do
		STATE=$(strings $rproc_dir/state)
		if test "${STATE}" == "running" ;then
			echo "M4 application '${APPLICATION_NAME}' started"
			exit 0
		else
			sleep 1
		fi
	done
	echo "WARNING: M4 application '${APPLICATION_NAME}' start timeout"
}

do_stop ()
{
	echo -n stop > $rproc_dir/state 2>/dev/null
	echo "M4 application stopped"
}

do_usage()
{
	echo "Usage: m4-autoload start|stop|restart [APPLICATION]"
}

do_parse()
{
	CMD=""
	while test "$1" != ""; do
		case $1 in
		start)
			CMD="start"
			;;
		stop)
			CMD="stop"
			;;
		restart)
			CMD="restart"
			;;
		-*| --* | -h | --help | help)
			do_usage
			exit 0
			;;
		*)
			APPLICATION_NAME="${1}"
			do_find_application
			;;
		esac
		shift
	done
}


main ()
{
	# Parse commands
	do_parse $*

	# Execute
	case ${CMD} in
	start)
		do_start
		;;
	stop)
		do_stop
		;;
	restart)
		do_stop
		do_start
		;;
	esac

	exit 0
}

main $*
