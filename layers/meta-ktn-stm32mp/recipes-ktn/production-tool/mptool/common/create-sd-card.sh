#!/bin/bash
#=======================================================================
#
#          FILE: create-sd-card
#
#         USAGE: sudo ./create-sd-card
#
#   DESCRIPTION: generate recpective sd card with partitions and images 
#                defined in config
#
#=======================================================================

##############################################################################
# Set configuration defaults
##############################################################################
FW_DIR="."
IMG_NAME=""
PROD_IMG="y"
PROD_IMG_EXTRA_SIZE="300"
PARTITION_LAYOUT="TFA TFA UBOOT BOROOTFS USERFS RESCUEFS"
PARTITION_LAYOUT_ABBOOT="TFA TFA UBOOT BOOT ROOT_A ROOT_B USERFS"
TFA_IMG="tf-a-unknown.stm32"
TFA_IMG_SIZE="256"
UBOOT_IMG="u-boot-unknown.stm32"
UBOOT_SIZE="2"
BOROOTFS_IMG="borootfs-unknown.tar.gz"
USERFS_IMG="userfs-unknown.tar.gz"
RESCUEFS="n"
RESCUEFS_IMG="rescuefs-unknown.tar.gz"
RESCUEFS_SIZE="45"
BOOT_IMG="boot-unknown.tar.gz"
ROOT_A_IMG="root_A-unknown.tar.gz"
ROOT_B_IMG="root_B-unknown.tar.gz"

# There are two modes supported: EXTLINUX and ABBOOT
# EXTLINUX is default (for compatibility)
MODE=EXTLINUX

CONFIG_PATH="sd-card.layout"
PROD_IMG="y"
VERBOSE=""

#=======================================================================
#	reads config file
#	
#	If config file is set by command line use this, else use 
#	default config file
#=======================================================================
read_config_file ()
{
    while test "$1" != ""; do
	case ${1} in
	-c)
		CONFIG_PATH=${2}
		IMG_NAME=$(basename ${2} .layout).sdcard
		shift
		;;
	esac
	shift
    done
		
    #source config
    if test -f ${CONFIG_PATH} ;then
	    . $(realpath ${CONFIG_PATH})
    else
	    echo "ERROR: config file <${CONFIG_PATH}> not found"
	    exit 1
    fi

	# Verify mode setting
	case "${MODE}" in
	ABBOOT)
		PARTITION_LAYOUT="${PARTITION_LAYOUT_ABBOOT}"
		;;
	EXTLINUX);;
	*)
		echo "Invalid mode setting: ${MODE}"
		exit 1
	esac
}

#=======================================================================
#	user interface shows help text
#	input: -
#	return: -
#=======================================================================
ui_show_help ()
{
	echo "Usage:"
	echo "  create-sd-card.sh [FLAGS]"
	echo 
	echo "Flags and options"
	echo "  Flags or options on commandline have higher priority"
	echo "  -p y|n               Expand userfs and copy tar files to it (default y)"
	echo "  -c <FILENAME>        Use alternate config file (default ist /etc/mptool.config)"
	echo "  -u <DIRECTORY>       Set base directory for update files (default is FW_DIR)"
	echo "  -n <FILENAME>        Set sd image output name (default is IMG_NAME)"
	echo ""
	echo "SD card image is placed alongside the tar files in FW_DIR"
	echo ""
}

#=======================================================================
#	parse command line and set environment variable from arguments
#	Exits program if command line parsing fails
#	input: command line arguments
#	return: -
#=======================================================================
parse_cmdline()
{
    while test "$1" != ""; do

	#echo "Processing parameter <${1}> / $*" 
	
	case ${1} in
	-h | --help)
		ui_show_help
		exit 0
		;;
	-u)
		FW_DIR=${2}
		shift
		;;
	-n)
		IMG_NAME=${2}
		shift
		;;
		
	-p)
		PROD_IMG=${2}
		shift
		;;
	-c)
		# Configfile was parsed already earlier
		shift
		;;
	-v)
		# Verbose switch
		VERBOSE=1
		;;
	-*)
		echo "ERROR: Invalid option <${1}>"
		exit 1
		;;
	*)
		echo "ERROR: Invalid option <${1}>"
		exit 1
		;;
	esac
	shift
    done
    

}




#=======================================================================
#
# get sd* device
#
# outputs sd device to use
#=======================================================================
decide_sd_device () {
    local  __resultvar=$1
    
    local mount_device=$(mount | grep "on / type" | cut -d' ' -f1 )        
    mount_device=${mount_device%?}
    
    local sd_device
    local confirmation=""
    
    
    
    while test "$confirmation" != "yes"
    do
	ls /dev | grep sd 
	echo "select sd card device"
	read sd_device
	
	
	if [ "/dev/$sd_device" = "$mount_device" ]
	then
	    echo "================================"
	    echo "ERROR: selected device is rootfs"
	    echo "================================"
	else
	    echo "confirm device $sd_device"
	    read confirmation
	fi
	
    done
    sd_device="/dev/$sd_device"
    
    eval $__resultvar="'$sd_device'"
}

#=======================================================================
#
# check_if_rescueos
#
# check recuefs var and modify PARTITION LAYOUT
#=======================================================================
check_if_rescueos () {
    local __resultvar=$1
    local checkvar=$2
    local i=0
    
    
    if test "$checkvar" != "y"
    then
	for val in ${PARTITION_LAYOUT} ;do
	    
	    if test "$val" != "RESCUEFS" ;then
		part_array[$i]=$val
		i=$((i+1))
	    fi
	done
    else
	for val in ${PARTITION_LAYOUT} ;do
	    part_array[$i]=$val
	    i=$((i+1))
	done
    fi
    
    eval $__resultvar="'${part_array[*]}'"
}
#=======================================================================
#
# check_all_imgs
#
# checks img from part_array
#=======================================================================
check_all_imgs () {
    local local_parts=$*
    local i=0
    local img="_IMG"
    
    echo "Checking image files ..."
    for part in ${local_parts[@]} 
    do
	# Generate list for image contents
	combined_name="${part}${img}"
	if test -z "${!combined_name}"; then
	    PART_IMG[i]=":empty:"
	else
	    PART_IMG[i]=$FW_DIR/${!combined_name}
	fi

	# Generate list for production files
	combined_name_prod="${part}${img}_PROD"
	if test -v "${combined_name_prod}"; then
	    echo "   ${combined_name_prod} is set"
	    if test -z "${!combined_name_prod}"; then
		PART_IMG_PROD[i]=":empty:"
	    else
		PART_IMG_PROD[i]=$FW_DIR/${!combined_name_prod}
	    fi
	else
	    echo "   ${combined_name_prod} is not set, using sdcard defaults"
	    PART_IMG_PROD[i]=${PART_IMG[i]}
	fi

	i=$((i+1))
    done

    # Check existence of files
    echo "Checking if required files exist"
    for file in ${PART_IMG[@]} ${PART_IMG_PROD[@]}; do
	if test "${file}" = ":empty:"; then
	    continue
	fi
	if test ! -f ${file}; then
	    echo "   ERROR: image file <${file}> is missing"
	    exit 1
	else
	    echo "   OK: ${file}"
	fi
    done


    i=0
    echo "Partition contents:"
    for part in ${local_parts[@]} ; do
	echo "   ${part}: ${PART_IMG[${i}]} (prod ${PART_IMG_PROD[${i}]})"
	i=$((i+1))
    done
}
#=======================================================================
#
# get partition size
#
# 
#=======================================================================
get_partition_size () {
    local local_parts=$*
    local i=0
    local img="_SIZE"
    
    for part in ${local_parts[@]} 
    do
	combined_name="$part$img"
	PART_SIZE[i]=${!combined_name}
	i=$((i+1))
    done
}
#=======================================================================
#
# unmount all partitions of dev
#
# 
#=======================================================================
unmount_all_parts () {
    local device=$1
    local i=1
    local dev_short=$(echo "${device}" | cut -d '/' -f3)
    local current_parts="$(ls /dev | grep $dev_short | awk 'NR=='$i'')" 
    
    while test "$current_parts" != "" 
    do
	if test -e "/dev/${current_parts}" ; then
	    umount "/dev/$current_parts" 1>/dev/null 2>&1
	fi
	i=$((i+1))
	
	current_parts="$(ls /dev | grep $dev_short | awk 'NR=='$i'')"
	
    done
        
}
#=======================================================================
#
# do partiotions
#
# 
#=======================================================================

do_sd_partitions () {
    local __retdev=$1
    local sd_img_dev=""
    local i=0
    local tfa_index=1
    local tmp_dir=$(mktemp -d)
    local tmp_file="${tmp_dir}/sfdisk.layout"
    local size=0
    local status
    
    echo "Create partitions ..."
    
    echo "label: gpt" > $tmp_file 
    for part in ${PART_ARRAY[@]} 
    do
	#echo "$part"
	if test "$part" == "TFA" 
	then
	    echo "size=256KiB, name=fsbl${tfa_index}" >> $tmp_file
	    tfa_index=$((tfa_index+1))
	    size=$((size+256))
	elif test "$part" == "UBOOT"
	then
	    echo "size=${PART_SIZE[i]}MiB, name=ssbl" >> $tmp_file
	    size=$((size+(PART_SIZE[i]*1024)))
	elif test "$part" == "BOROOTFS"
	then
	    echo "size=${PART_SIZE[i]}MiB, name=boroot , attrs=\"LegacyBIOSBootable\"" >> $tmp_file
	    size=$((size+(PART_SIZE[i]*1024)))
	elif test "$part" == "BOOT"
	then
	    echo "size=${PART_SIZE[i]}MiB, name=boot , attrs=\"LegacyBIOSBootable\"" >> $tmp_file
	    size=$((size+(PART_SIZE[i]*1024)))
	elif test "$part" == "ROOT_A"
	then
	    echo "size=${PART_SIZE[i]}MiB, name=root_A" >> $tmp_file
	    size=$((size+(PART_SIZE[i]*1024)))
	elif test "$part" == "ROOT_B"
	then
	    echo "size=${PART_SIZE[i]}MiB, name=root_B" >> $tmp_file
	    size=$((size+(PART_SIZE[i]*1024)))
	elif test "$part" == "USERFS"
	then
	    if test "$PROD_IMG" = "y"
	    then
		echo "size=$((PART_SIZE[i]+PROD_IMG_EXTRA_SIZE))MiB, name=userfs" >> $tmp_file
		size=$((size+(PART_SIZE[i]+PROD_IMG_EXTRA_SIZE)*1024))
	    else
		echo "size=${PART_SIZE[i]}MiB, name=userfs" >> $tmp_file
		size=$((size+(PART_SIZE[i]*1024)))
	    fi
	    
	elif test "$part" == "RESCUEFS"
	then
	    echo "size=${PART_SIZE[i]}MiB, name=rescue-os" >> $tmp_file
	    size=$((size+(PART_SIZE[i]*1024)))
	fi
	i=$((i+1))
    done
    
    #create image
    echo "   Calculated image size: $size kiB"
    if test -e ${FW_DIR}/${IMG_NAME}
    then
	rm -rf ${FW_DIR}/{$IMG_NAME}
    fi
    dd if=/dev/zero of=${FW_DIR}/${IMG_NAME} bs=1k count=${size} 1>/dev/zero 2>&1
    
    sd_img_dev="$(losetup --show -f -P $FW_DIR/$IMG_NAME)"
    
    sfdisk -f -q $sd_img_dev < $tmp_file > /dev/null
    
    partprobe -d $sd_img_dev 1>/dev/zero 2>&1
    
    eval $__retdev="'${sd_img_dev}'"
    
    rm -rf ${tmp_dir}
}

#=======================================================================
#
# flash partitions
# 
#=======================================================================
flash_partitions () {
    local ret_val
    local device=$1
    local tmp_dir=$(mktemp -d)
    local mnt_point="${tmp_dir}/sdcard-mount/"
    local part_label="p"
    mkdir -p $mnt_point 1>/dev/null 2>&1
    local array_index=0
    local dev_index=1
    local prod_index=0
    local status=0
    
    echo "Fill partitions ..."
    for part in ${PART_ARRAY[@]} 
    do
	if [ "$part" == "TFA" -o "$part" == "UBOOT" ]; then
	    if test "${PART_IMG[array_index]}" = ":empty:"; then
		echo "   Skip filling ${part_array[array_index]} (part $dev_index)"
	    else
		echo "   Filling ${part_array[array_index]} (part $dev_index) with ${PART_IMG[array_index]}"
		dd if=${PART_IMG[array_index]} of="${device}p$dev_index" conv=notrunc 1>/dev/null 2>&1
	    fi
	else
	    mkfs.ext4 -I 256 -F "${device}p$dev_index" 1>/dev/null 2>&1
	    mkdir "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
	    mount "${device}p$dev_index" "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
	    ret_val=$?
	    if test "$ret_val" != 0
	    then
		echo "problem with $part partition"
		
		rm -rf "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
		losetup -d "${device}"
		rm -rf $mnt_point 1>/dev/null 2>&1
		rm -rf ${tmp_dir}
		exit 1
	    else
		if test "${PART_IMG[array_index]}" = ":empty:"; then
		    echo "   Skip filling ${part_array[array_index]} (part $dev_index)"
		else
		    echo "   Filling ${part_array[array_index]} (part $dev_index) with ${PART_IMG[array_index]}"
		    tar xf ${PART_IMG[array_index]} -C "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
		    status=$?
		    if test $status != 0; then
			echo "failed to burn ${PART_IMG[array_index]} to Partition $dev_index "
			echo "Image partition size (${PART_SIZE[array_index]}MB) might not be enough"
			echo "try increasing it"
			umount "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
			rm -rf "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
			losetup -d "${device}"
			rm -rf $mnt_point 1>/dev/null 2>&1
			rm -rf ${tmp_dir}
			exit 1
		    fi
		fi
	    fi
	    #prod img copy all images to userfs
	    if [ "$PROD_IMG" == "y" ]; then
		if [ "$part" == "USERFS" ]; then
		    echo "Extending production image ..."
		    for part in ${PART_ARRAY[@]} 
		    do
			if test "${PART_IMG_PROD[prod_index]}" != ":empty:"; then
			    echo "   Writing ${PART_IMG_PROD[prod_index]}"
			    cp ${PART_IMG_PROD[prod_index]} "${mnt_point}/${dev_index}"
			fi
			prod_index=$((prod_index+1))
		    done
		    echo "   Modify mptool.config"
		    create_mptool_config "${mnt_point}/${dev_index}/mptool.config"
		fi
	    fi
	    umount "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
	    rm -rf "${mnt_point}/${dev_index}" 1>/dev/null 2>&1
	fi
	array_index=$((array_index+1))
	dev_index=$((dev_index+1))
    done
    losetup -d "${device}"
    rm -rf $mnt_point 1>/dev/null 2>&1
    rm -rf ${tmp_dir}
}

check_user_root ()
{
    if test "${USER}" != "root"; then
	echo "ERROR: Script must be run as user 'root'. Please use sudo or something similar!" >&2
	exit 1
    fi
}

create_mptool_config ()
{
    local CONFIGFILE=${1}
    
    (
	# Use subshell not to destroy script environment
	
	# Set dynamic defaults
	if test ! -v TFA_IMG_PROD; then
	    # not set -> set defaults
	    TFA_IMG_PROD=${TFA_IMG}
	fi
	if test ! -v UBOOT_IMG_PROD; then
	    # not set -> set defaults
	    UBOOT_IMG_PROD=${UBOOT_IMG}
	fi
	if test ! -v BOROOTFS_IMG_PROD; then
	    # not set -> set defaults
	    BOROOTFS_IMG_PROD=${BOROOTFS_IMG}
	fi
	if test ! -v USERFS_IMG_PROD; then
	    # not set -> set defaults
	    USERFS_IMG_PROD=${USERFS_IMG}
	fi
	if test ! -v RESCUEFS_IMG_PROD; then
	    # not set -> set defaults
	    RESCUEFS_IMG_PROD=${RESCUEFS_IMG}
	fi
	if test ! -v BOOT_IMG_PROD; then
	    # not set -> set defaults
	    BOOT_IMG_PROD=${BOOT_IMG}
	fi
	if test ! -v ROOT_A_IMG_PROD; then
	    # not set -> set defaults
	    ROOT_A_IMG_PROD=${ROOT_A_IMG}
	fi
	if test ! -v ROOT_B_IMG_PROD; then
	    # not set -> set defaults
	    ROOT_B_IMG_PROD=${ROOT_B_IMG}
	fi

	echo "# Generated file from create-sd-card.sh script"
	#echo "#"
	#echo "#TFA_IMG_PROD=${TFA_IMG_PROD}"
	#echo "#UBOOT_IMG_PROD=${UBOOT_IMG_PROD}"
	#echo "#BOROOTFS_IMG_PROD=${BOROOTFS_IMG_PROD}"
	#echo "#USERFS_IMG_PROD=${USERFS_IMG_PROD}"
	#echo "#RESCUEFS_IMG_PROD=${RESCUEFS_IMG_PROD}"
	#echo "#BOOT_IMG_PROD=${BOOT_IMG_PROD}"
	#echo "#ROOT_A_IMG_PROD=${ROOT_A_IMG_PROD}"
	#echo "#ROOT_B_IMG_PROD=${ROOT_B_IMG_PROD}"
	echo ""
	echo "# bootloaders"
	echo "TFA_IMG=\"$(basename ${TFA_IMG_PROD})\""
	echo "UBOOT_IMG=\"$(basename ${UBOOT_IMG_PROD})\""
	echo 
	if test "${MODE}" != "ABBOOT"; then
		echo "# boot + rootfs (SIZE in MiB)"
		echo "BOROOTFS_IMG=\"$(basename ${BOROOTFS_IMG_PROD})\""
		echo 
		echo "# userfs (if size empty -> all remaining space taken else SIZE in MiB)"
		echo "USERFS_IMG=\"$(basename ${USERFS_IMG_PROD})\""
		echo
	else
		echo "FS_PARTITION_LAYOUT=\"boot root_A root_B userfs\""
		echo ""
		echo "# boot (SIZE in MiB)"
		echo "boot_IMG=\"$(basename ${BOOT_IMG_PROD})\""
		echo ""
		echo "# root_A (SIZE in MiB)"
		echo "root_A_IMG=\"$(basename ${ROOT_A_IMG_PROD})\""
		echo ""
		echo "# root_B (SIZE in MiB)"
		echo "root_B_IMG=\"$(basename ${ROOT_B_IMG_PROD})\""
		echo ""
		echo "# userfs (SIZE in MiB)"
		echo "userfs_IMG=\"$(basename ${USERFS_IMG_PROD})\""
		echo
	fi

    ) > ${CONFIGFILE}
    
    if test -n "${VERBOSE}"; then
	echo "==== Begin mptool.cfg ===="
	cat ${CONFIGFILE}
	echo "==== End mptool.cfg ===="
    fi
}


show_configuration ()
{
    echo "Selected configuration:"
    echo "  FW_DIR              ${FW_DIR}"
    echo "  IMG_NAME            ${IMG_NAME}"
    echo "  PROD_IMG            ${PROD_IMG}"
    echo "  RESCUEFS            ${RESCUEFS}"
    echo ""
    echo "Partition contens:"
    echo "  TFA_IMG             ${TFA_IMG}"
    echo "  UBOOT_IMG           ${UBOOT_IMG}"
    echo "  BOROOTFS_IMG        ${BOROOTFS_IMG}"
    echo "  USERFS_IMG          ${USERFS_IMG}"
    echo "  RESCUEFS_IMG        ${RESCUEFS_IMG}"
    echo ""
    echo "Partition layouts:"
    echo "  PARTITION_LAYOUT    ${PARTITION_LAYOUT}"
    echo "  TFA_IMG_SIZE        ${TFA_IMG_SIZE}"
    echo "  UBOOT_SIZE          ${UBOOT_SIZE}"
    echo "  BOROOTFS_SIZE       ${BOROOTFS_SIZE}"
    echo "  USERFS_SIZE         ${USERFS_SIZE}"
    echo "  PROD_IMG_EXTRA_SIZE ${PROD_IMG_EXTRA_SIZE}"
    echo "  RESCUEFS_SIZE       ${RESCUEFS_SIZE}"
    echo ""
    echo "Production configuration:"
    if test ! -v TFA_IMG_PROD; then
	# not set -> set defaults
	echo "  TFA_IMG_PROD        -> sdcard contents"
    else
	echo "  TFA_IMG_PROD        ${TFA_IMG_PROD}"
    fi

    if test ! -v UBOOT_IMG_PROD; then
	# not set -> set defaults
	echo "  UBOOT_IMG_PROD      -> sdcard contents"
    else
	echo "  UBOOT_IMG_PROD      ${UBOOT_IMG_PROD}"
    fi

    if test ! -v BOROOTFS_IMG_PROD; then
	# not set -> set defaults
	echo "  BOROOTFS_IMG_PROD   -> sdcard contents"
    else
	echo "  BOROOTFS_IMG_PROD   ${BOROOTFS_IMG_PROD}"
    fi

    if test ! -v USERFS_IMG_PROD; then
	# not set -> set defaults
	echo "  USERFS_IMG_PROD     -> sdcard contents"
    else
	echo "  USERFS_IMG_PROD     ${USERFS_IMG_PROD}"
    fi

    if test ! -v RESCUEFS_IMG_PROD; then
	# not set -> set defaults
	echo "  RESCUEFS_IMG_PROD   -> sdcard contents"
    else
	echo "  RESCUEFS_IMG_PROD   ${RESCUEFS_IMG_PROD}"
    fi
    
    echo ""
}


#=======================================================================
#
# main
#
# 
#=======================================================================


main () {
    read_config_file $*
    
    parse_cmdline $*
    
	echo "Selected image mode is ${MODE}"

    if test -n "${VERBOSE}"; then
	show_configuration
    fi
    
    check_user_root
    
    check_if_rescueos PART_ARRAY $RESCUEFS
    
    check_all_imgs ${PART_ARRAY[*]}
    
    get_partition_size ${PART_ARRAY[*]}
    
    do_sd_partitions IMG_DEVICE 
    
    flash_partitions $IMG_DEVICE
    
    echo ""
    echo "Created image file: ${FW_DIR}/${IMG_NAME}"
    echo ""
    echo "You can burn this image to your sd card with this command"
    echo "> dd if=${FW_DIR}/${IMG_NAME} bs=8M conv=fdatasync of=/dev/sd-yourdevice"
    echo ""
    echo "ATTENTION !!!"
    echo "    Be careful! Use the correct device file for your sd card else you may"
    echo "    DAMAGE your system!"
    echo ""
    
}
#=======================================================================
#
# 
#
# 
#=======================================================================

main $*
