#!/bin/sh

stop_splash_path="/usr/bin/psplash-drm-quit"
script_path="/usr/bin/autostart-app.sh"
pidfile="/run/autostart-app.sh.pid"

check_active ()
{
	if test -e ${pidfile}; then
		read pid < ${pidfile}
		check_active__pid=${pid}
		kill -s 0 ${pid} 2> /dev/null
		if test "$?" = "0"; then
			check_active__is_active=1
		else
			check_active__is_active=0
			rm ${pidfile}
		fi
	fi
}

case "$1" in

start)
	if test -f $stop_splash_path;then
		$stop_splash_path &
	fi

	check_active
	if test "${check_active__is_active}" = "1"; then
		echo "already active (PID ${check_active__pid})"
		exit 0
	fi

	if test -f $script_path;then
		$script_path 1>/dev/null 2>&1 &
		pid=$!
		echo "activated (PID ${pid})"
		echo ${pid} > ${pidfile}
	fi
	;;

stop)
	if test -e ${pidfile}; then
		read pid < ${pidfile}
		echo "stoped (PID ${pid})"
		kill ${pid}
		rm ${pidfile}
	fi
	exit 0
	;;

status)
	check_active
	if test "${check_active__is_active}" = "1"; then
		echo "active (PID ${check_active__pid})"
		exit 0
	fi
	echo "inactive"
	;;

*)
	echo "Unknown command <${1}>"
	exit 1
	;;

esac
