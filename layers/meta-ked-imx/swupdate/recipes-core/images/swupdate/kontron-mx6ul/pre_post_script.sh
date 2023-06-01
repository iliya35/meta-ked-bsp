#!/bin/sh

mnt_dir=/tmp/mnt


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
	#mount and delete rootfs which will be updated
	if [ "$rootfs_partition" == "ubi0:root_A" ] ;then
		mount -t ubifs /dev/ubi0_1 ${mnt_dir}
		if [ $? -ne 0 ];then  
			echo "failed to mount ubifs from /dev/ubi0_2"
			return 1
		fi
	else
		mount -t ubifs /dev/ubi0_2 ${mnt_dir}
		if [ $? -ne 0 ];then  
			echo "failed to mount ubifs from /dev/ubi0_1"
			return 1
		fi
	fi
	
	echo "Deleting old rootfs: $rootfs_partition"
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
	mount -t ubifs /dev/ubi0_0 ${mnt_dir}
	status=$?

	if test "$status" != "0" ;then
		echo "failed to mount ubifs from /dev/ubi0_0"
		return 1
	fi

	#set fallback partition	
	if test "${cur_par}" == "ubi0:root_A" ;then
		echo "Set Fallback root partition to A"
		set_sys_active "AA"
	else
		echo "Set Fallback root partition to B"
		set_sys_active "BB"
	fi

	#move active kernel to inactive
	if [ -e ${mnt_dir}/fitImage_active ]; then
		echo "Moving fitImage_active to inactive"
		mv ${mnt_dir}/fitImage_active ${mnt_dir}/fitImage_inactive
	fi

	#set active partition
	if test "${cur_par}" == "ubi0:root_A" ;then
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
	
	if [ ! -e /dev/ubi0 ]; then
		ubiattach -m 0 
		status=$?
	fi
	
	if [ $status -ne 0 ]; then
		echo "failed to attach UBI device"
		return 1
	fi
	
	get_active_partition
	
	if [ "${cur_par}" == "ubi0:root_A" ];then
		delete_old_rootfs_content "ubi0:root_B"
		status=$?
		
	fi
	
	if [ "${cur_par}" == "ubi0:root_B" ];then
		delete_old_rootfs_content "ubi0:root_A"
		status=$?
	fi
	
	# delete only partition A if we have bootet not from flash  (e.g. sdcard or Network - needed for initial partitioning / setup)
	if [ ! "${cur_par}" == "ubi0:root_A" -a ! "${cur_par}" == "ubi0:root_B" ];then
        	delete_old_rootfs_content "ubi0:root_A"
        	status=$?

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
