#!/bin/sh

mnt_dir=/tmp/mnt

bootfs="/dev/mmcblk0p1"
rootfs_a="/dev/mmcblk0p2"
rootfs_b="/dev/mmcblk0p3"


###################################################
# Create tempdir and 
#		determine current rootfs partition
###################################################
get_active_partition() {

	if [ ! -e ${mnt_dir} ];then 
		mkdir ${mnt_dir}
	fi  

	#get rootfs block device 		
	cur_par=$(findmnt -n -o SOURCE /)
	echo "current partition ${cur_par}"
}

###################################################
# Delete content of old rootfs
###################################################
delete_old_rootfs_content() {
	
	local rootfs_partition=$1
	
	if [ -z "$rootfs_partition" ];then 
		echo "parameter of partition required"
		return 1
	fi 
	
	mount -t ext4 ${rootfs_partition} ${mnt_dir}
	if [ $? -ne 0 ];then  
		echo "failed to mount ${rootfs_partition}}"
		return 1
	fi

	echo "Deleting old rootfs: ${rootfs_partition}"
	rm -rf ${mnt_dir}/*	
	umount ${mnt_dir}/
	return 0

}

###################################################
# Set content of sys_active file atomically
###################################################
set_sys_active() {
	echo $1 > ${mnt_dir}/sys_active_tmp
	mv ${mnt_dir}/sys_active_tmp ${mnt_dir}/sys_active
}

###################################################
#	Correct switching of FitImage 
#	and current partition file
###################################################
do_switch_fitImage() {
	local status=0
	mount -t ext4 ${bootfs} ${mnt_dir}
	status=$?

	if test "$status" != "0" ;then
		echo "failed to mount ${bootfs}"
		return 1
	fi

	#set fallback partition	
	if test "${cur_par}" == ${rootfs_a} ;then
		echo "Set Fallback root partition to A"
		set_sys_active "AA"
	else
		echo "Set Fallback root partition to B"
		set_sys_active "BB"
	fi

	#set active partition
	if test "${cur_par}" == ${rootfs_a};then
		echo "Set active root partition to B with Fallback on A"
		set_sys_active "BA"
	else
		echo "Set active root partition to A with Fallback on B"
		set_sys_active "AB"
	fi

	umount ${mnt_dir}
}

###################################################
# Preinstall script
###################################################
do_preinst() {
	local status=0
	echo "preinstall"
	
	get_active_partition
	
	if [ "${cur_par}" == ${rootfs_a} ];then
		delete_old_rootfs_content ${rootfs_b}
		status=$?
	
	elif [ "${cur_par}" == ${rootfs_b} ];then
		delete_old_rootfs_content ${rootfs_a}
		status=$?
	else
		return 0
	fi
	
	
	if [ $status -ne 0 ]; then
		echo "failed to delete old rootfs content"
		return 1
	fi
	
	do_switch_fitImage
	status=$?
	
	if [ $status -ne 0 ]; then
	    echo "failed to switch fitImage from active to inactive"
		return 1
	fi
	
	
	return 0
}

###################################################
# postinstall script
###################################################
do_postinst() {
	echo "postinstall"
	return 0
}


case "$1" in
preinst)
	echo "call do_preinst"
	fw_setenv upgrade_available 1
	fw_setenv bootcount 0
	do_preinst
	;;
postinst)
	echo "call post"
	do_postinst
	;;
esac
