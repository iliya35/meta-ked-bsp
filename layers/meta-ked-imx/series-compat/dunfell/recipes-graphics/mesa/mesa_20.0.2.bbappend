FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Add Etnaviv Patch as described here: https://lists.freedesktop.org/archives/dri-devel/2020-December/291458.html
SRC_URI:append = " file://etnaviv-pass-correct-layout-to-etna_resource_alloc-for-scanout-resources.patch"
