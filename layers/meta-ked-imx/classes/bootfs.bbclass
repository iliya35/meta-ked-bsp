IMAGE_BOOTFS ?= "${IMAGE_ROOTFS}/../bootfs"
IMAGE_BOOTFS_NAME ?= "${IMAGE_BASENAME}-bootfs-${MACHINE}-${DISTRO_CODENAME}"

do_bootfs() {
	if [ -d ${IMAGE_BOOTFS} ]; then
		rm -rf ${IMAGE_BOOTFS}/*
	else
		mkdir ${IMAGE_BOOTFS}
	fi

	cp -L ${DEPLOY_DIR_IMAGE}/fitImage ${IMAGE_BOOTFS}/fitImage_active
	cp -L ${DEPLOY_DIR_IMAGE}/fitImage ${IMAGE_BOOTFS}/fitImage_inactive
	echo "AB" > ${IMAGE_BOOTFS}/sys_active

	if [ -e ${IMGDEPLOYDIR}/${IMAGE_BOOTFS_NAME}.tar.gz ];then
		rm ${IMGDEPLOYDIR}/${IMAGE_BOOTFS_NAME}.tar.gz
	fi
	cd ${IMAGE_BOOTFS}
	tar cvfz ${IMGDEPLOYDIR}/${IMAGE_BOOTFS_NAME}.tar.gz ./*
}

addtask do_bootfs after do_rootfs before do_image
