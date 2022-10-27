# =========================================================================
# some basic settings copied from poky
# =========================================================================

DISTRO_VERSION[vardepsexclude] = "DATE"
SDK_VERSION[vardepsexclude] = "DATE"

# We only use glibc and we don't need to append to the TMPDIR variable
TCLIBCAPPEND = ""

PREMIRRORS ??= "\
bzr://.*/.*   http://downloads.yoctoproject.org/mirror/sources/ \n \
cvs://.*/.*   http://downloads.yoctoproject.org/mirror/sources/ \n \
git://.*/.*   http://downloads.yoctoproject.org/mirror/sources/ \n \
gitsm://.*/.* http://downloads.yoctoproject.org/mirror/sources/ \n \
hg://.*/.*    http://downloads.yoctoproject.org/mirror/sources/ \n \
osc://.*/.*   http://downloads.yoctoproject.org/mirror/sources/ \n \
p4://.*/.*    http://downloads.yoctoproject.org/mirror/sources/ \n \
svn://.*/.*   http://downloads.yoctoproject.org/mirror/sources/ \n"

MIRRORS =+ "\
ftp://.*/.*      http://downloads.yoctoproject.org/mirror/sources/ \n \
http://.*/.*     http://downloads.yoctoproject.org/mirror/sources/ \n \
https://.*/.*    http://downloads.yoctoproject.org/mirror/sources/ \n"

# The CONNECTIVITY_CHECK_URI's are used to test whether we can succesfully
# fetch from the network (and warn you if not). To disable the test set
# the variable to be empty.
# Git example url: git://git.yoctoproject.org/yocto-firewall-test;protocol=git;rev=master
CONNECTIVITY_CHECK_URIS ?= "https://www.google.com/"

SANITY_TESTED_DISTROS ?= " \
            poky-2.7 \n \
            poky-3.0 \n \
            poky-3.1 \n \
            ubuntu-16.04 \n \
            ubuntu-18.04 \n \
            ubuntu-19.04 \n \
            ubuntu-20.04 \n \
            fedora-30 \n \
            fedora-31 \n \
            fedora-32 \n \
            fedora-33 \n \
            fedora-34 \n \
            fedora-35 \n \
            centos-7 \n \
            centos-8 \n \
            debian-8 \n \
            debian-9 \n \
            debian-10 \n \
            debian-11 \n \
            opensuseleap-15.1 \n \
            opensuseleap-15.2 \n \
            opensuseleap-15.3 \n \
            almalinux-8.5 \n \
            "
#
# OELAYOUT_ABI allows us to notify users when the format of TMPDIR changes in
# an incompatible way. Such changes should usually be detailed in the commit
# that breaks the format and have been previously discussed on the mailing list
# with general agreement from the core team.
#
OELAYOUT_ABI = "12"

# QA check settings - a little stricter than the OE-Core defaults
WARN_TO_ERROR_QA = "already-stripped compile-host-path install-host-path \
                    installed-vs-shipped ldflags pn-overrides rpaths staticdev \
                    unknown-configure-option useless-rpaths"
WARN_QA:remove = "${WARN_TO_ERROR_QA}"
ERROR_QA:append = " ${WARN_TO_ERROR_QA}"

require conf/distro/include/no-static-libs.inc
require conf/distro/include/yocto-uninative.inc
require conf/distro/include/security_flags.inc
INHERIT += "uninative"

INHERIT += "reproducible_build"

BB_SIGNATURE_HANDLER ?= "OEBasicHash"
BB_HASHSERVE ??= ""
