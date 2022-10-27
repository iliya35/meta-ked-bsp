SUMMARY = "Package to provide a script for generating QtCreator Kits"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

EXCLUDE_FROM_WORLD = "1"
MODIFYTOS = "0"

REAL_MULTIMACH_TARGET_SYS = "${TUNE_PKGARCH}${TARGET_VENDOR}-${TARGET_OS}"

SDK_DIR = "${WORKDIR}/sdk"
SDK_OUTPUT = "${SDK_DIR}/image"
SDKTARGETSYSROOT = "${SDKPATH}/sysroots/${REAL_MULTIMACH_TARGET_SYS}"

SRC_URI = "file://qtcreator-setup-sdk.sh.part"

inherit cross-canadian

do_create_sdk_files[dirs] = "${SDK_OUTPUT}/${SDKPATH}"
do_create_sdk_files() {
    script=${SDK_OUTPUT}/${SDKPATH}/qtcreator-setup-helper-${REAL_MULTIMACH_TARGET_SYS}.sh
    rm -f $script
    touch $script

    echo '#!/bin/bash' >> $script
    echo '' >> $script
    echo 'SDK_NAME="${DISTRO_CODENAME}-${SDK_VERSION}-${MACHINE}"' >> $script
    echo 'TC_PATH="${SDKPATHNATIVE}${bindir_nativesdk}/${TARGET_SYS}"' >> $script
    echo 'CXX_PATH="$TC_PATH/${TARGET_PREFIX}g++"' >> $script
    echo 'GDB_PATH="$TC_PATH/${TARGET_PREFIX}gdb"' >> $script
    echo 'QMAKE_PATH="${SDKPATHNATIVE}${bindir_nativesdk}/qmake"' >> $script
    echo 'SYSROOT_PATH="${SDKTARGETSYSROOT}"' >> $script

    if [ "${TUNE_ARCH}" = "arm" ]; then
        echo 'TC_ABI="arm-linux-generic-elf-32bit"' >> $script
    elif [ "${TUNE_ARCH}" = "aarch64" ]; then
        echo 'TC_ABI="arm-linux-generic-elf-64bit"' >> $script
    fi

    echo 'TC_ID_PREFIX="ProjectExplorer.ToolChain.Gcc"' >> $script
    echo 'ENV_SCRIPT="${SDKPATH}/environment-setup-${REAL_MULTIMACH_TARGET_SYS}"' >> $script
    echo '' >> $script
    cat ${WORKDIR}/qtcreator-setup-sdk.sh.part >> $script
    chmod +x $script
}
addtask create_sdk_files before do_install after do_patch

do_install() {
    install -d ${D}/${SDKPATH}
    install -m 0755 -t ${D}/${SDKPATH} ${SDK_OUTPUT}/${SDKPATH}/*
}

PN = "qtcreator-setup-helper-${MACHINE}"
PACKAGES = "${PN}"
INSANE_SKIP:${PN} = "file-rdeps"
FILES:${PN} = "${SDKPATH}/*"

deltask do_configure
deltask do_compile
deltask do_populate_sysroot
