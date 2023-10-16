FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://0001-libubootenv-support-newer-yaml-config.patch \
    file://0002-Always-link-libubootenv.patch \
"
