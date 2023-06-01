#
# With no window manager like X11 or Wayland available, we need
# to use the GBM backend in order to use OpenGL with GStreamer.
# Enabling it prevents a build error with gstreamer1.0-plugins-bad
# which expects the gstreamer-gl module to be avialable when opengl
# is enabled in DISTRO_FEATURES.
#
PACKAGECONFIG_GL:append = "${@bb.utils.contains('DISTRO_FEATURES', 'opengl', ' gbm', '', d)}"
