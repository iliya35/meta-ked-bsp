FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://oe-device-extra.pri;subdir=git/mkspecs"

# Add eglfs_viv and eglfs_kms backends. Make linuxfb the default QPA
PACKAGECONFIG_GL = " ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'gles2', '', d)} "
PACKAGECONFIG:append = " eglfs kms gbm examples accessibility "
QT_CONFIG_FLAGS += " -no-sse2 -no-opengles3 -qpa linuxfb"
