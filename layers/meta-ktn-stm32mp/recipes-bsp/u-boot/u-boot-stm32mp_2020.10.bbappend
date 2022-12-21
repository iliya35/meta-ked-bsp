U_BOOT_VERSION = "v2020.10"
U_BOOT_SUBVERSION = "stm32mp"
U_BOOT_RELEASE = "r2.1-ktn"

SRCREV = "d0fccae616fa39a443284648bd55491c9f4c2d05"
SRC_URI = "git://${KTN_GIT_SERVER}/sw/misc/u-boot.git;protocol=https;branch=develop-${U_BOOT_VERSION}-stm32mp-ktn"


do_buildenv[nostamp]="1"
do_buildenv() {
	{
		echo "if test -z \${DEFCONFIG}; then"
		echo "    export DEFCONFIG"
		echo "    echo \"ERROR: Set DEFCONFIG in your environment and source again!\""
		echo "    echo \"Currently available configs:\""
		echo "    for config in \$(ls -1d \${B}/*_defconfig); do"
		echo "        echo -n \"    export DEFCONFIG=\""
		echo "        basename \$config"
		echo "    done"
		echo "    unalias makey"
		echo "else"
		echo "    export CROSS_COMPILE=\$(echo ${CC} | sed -n \"s/gcc .*//p\")"
		echo "    export MF=\"-j6 ARCH=arm CROSS_COMPILE=\${CROSS_COMPILE} O=${B}/\${DEFCONFIG} \""
		echo "    export S=${S}"
		echo "    export O=${B}/\${DEFCONFIG}"
		echo "    export D=${D}"
		echo "    alias makey=\"make \${MF}\""
		echo "    ln -s \${O} O"
		echo "    echo Output directory O=${B}/\${DEFCONFIG}"
		echo "    echo Makeflags MF=\${MF}"
		echo "    echo CROSS_COMPILE=\${CROSS_COMPILE}"
		echo "fi"

	} > ${S}/recipe.env
}

addtask buildenv after do_configure before do_devshell
