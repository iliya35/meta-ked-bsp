#!/bin/sh

export XDG_RUNTIME_DIR=/tmp/runtime-root
export QT_QPA_PLATFORM
# screen width in millimeters
export DISPLAY_PHYS_WIDTH
export DISPLAY_PHYS_HEIGHT
# disable sandbox reqired when started as root
export QTWEBENGINE_DISABLE_SANDBOX=1
export QT_QPA_EGLFS_ALWAYS_SET_MODE=1

config_path="/etc/default/autostart-app"

if test -f $config_path;then
	. $config_path
fi

if test -z "${DISPLAY_PHYS_WIDTH}"; then
	# set defaults
	DISPLAY_PHYS_WIDTH=208
fi
if test -z "${DISPLAY_PHYS_HEIGHT}"; then
	# set defaults
	DISPLAY_PHYS_HEIGHT=130
fi

start_eglfs ()
{
	export QT_QPA_EGLFS_PHYSICAL_WIDTH=${DISPLAY_PHYS_WIDTH}
	export QT_QPA_EGLFS_PHYSICAL_HEIGHT=${DISPLAY_PHYS_HEIGHT}

	mkdir -p "${XDG_RUNTIME_DIR}"
	mkdir -p /tmp/.eglfs

	board_model=`cat /proc/device-tree/model`
	soc_model=""
	case $board_model in
	*i.MX6UL* )
		soc_model="mx6ul"
		;;
	*i.MX8MM* )
		if [ -d /sys/class/drm/card0/card0-LVDS-1 ] ||
		   [ -d /sys/class/drm/card0/card0-HDMI-A-1 ]; then
			echo '{"device": "/dev/dri/card0","hwcursor":false}' > /tmp/.eglfs/kms.json
		elif [ -d /sys/class/drm/card1/card1-LVDS-1 ] ||
		     [ -d /sys/class/drm/card1/card1-HDMI-A-1 ]; then
			echo '{"device": "/dev/dri/card1","hwcursor":false}' > /tmp/.eglfs/kms.json
		fi
		soc_model="mx8m"
		;;
	*stm32mp* )
		echo '{"device": "/dev/dri/card0","hwcursor":false}' > /tmp/.eglfs/kms.json
		soc_model="stm32mp"
		;;
	esac

	# Fall back to linuxfb backend and software rendering if no backend is
	# set or we don't have a GPU (i.MX6UL/ULL)
	if test -z "${QT_QPA_PLATFORM}" && test "$soc_model" = "mx6ul"; then
		echo "Running on platform without GPU, using linuxfb backend"
		export QT_QPA_PLATFORM=linuxfb
	elif test -z "${QT_QPA_PLATFORM}" && test "$soc_model" = "mx8m" && test -d /sys/class/drm/renderD128; then
		echo "Running on i.MX8M with etnaviv GPU, using eglfs backend"
		export QT_QPA_PLATFORM=eglfs
	elif test -z "${QT_QPA_PLATFORM}"; then
		echo "Falling back to default platform (linuxfb)"
		export QT_QPA_PLATFORM=linuxfb
		export QT_QPA_FB_DRM=1
	fi

	if test "${QT_QPA_PLATFORM}" = "linuxfb"; then
		echo "Activate software rendering"
		export QT_QUICK_BACKEND=software
		export QT_QPA_PLATFORM="linuxfb:mmsize=${DISPLAY_PHYS_WIDTH}x${DISPLAY_PHYS_HEIGHT}"
		#export QT_QPA_FB_DRM=1 # use DRM APIs for rendering
	fi

	export QT_QPA_EGLFS_KMS_CONFIG=/tmp/.eglfs/kms.json
	if test ! -x "${APPLICATION}"; then
		echo "ERROR: '${APPLICATION}' seems to not to be a valid application"
		exit 1
	fi
	if test -f /sys/class/graphics/fbcon/cursor_blink; then
		BLINK=$(cat /sys/class/graphics/fbcon/cursor_blink)
		echo 0 > /sys/class/graphics/fbcon/cursor_blink
	fi
	echo "Now starting application (${APPLICATION})"
	echo "  With options '${APPLICATION_OPTIONS}' "
	exec ${APPLICATION} ${APPLICATION_OPTIONS}
}

echo "Starting eglfs application ${APPLICATION}"
start_eglfs
exit $?
