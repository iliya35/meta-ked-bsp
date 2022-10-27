FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# meta-freescale uses OsMesa for i.MX8, but we want Gallium + etnaviv.
USE_OSMESA_ONLY_mx8 = "no"

# Add Etnaviv Patch as described here: https://lists.freedesktop.org/archives/dri-devel/2020-December/291458.html
SRC_URI:append = " file://etnaviv-pass-correct-layout-to-etna_resource_alloc-for-scanout-resources.patch"
