FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG = "gstreamer"
DEPENDS += "gstreamer1.0 gstreamer1.0-plugins-base"

SRC_URI:append = "  \
    file://0001-fix-gbm_bo_map-detection.patch \
    file://0001-Disable-Texturator.patch \
    file://0002-fix-gbm_bo_map-detection-for-MESON.patch \
"
