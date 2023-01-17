inherit userfs-settings

IMAGE_INSTALL += "mount-userfs-partition"

fakeroot do_userfs() {
    if [ ${IMAGE_CREATE_USERFS} -eq 0 ]; then
        exit 0
    fi

    # Check basic configuration
    if [ -z "${IMAGE_USERFS_MOUNT_LIST}" -a -z "${IMAGE_USERFS_LIST}" ]; then
        bbfatal "Please set either IMAGE_USERFS_MOUNT_LIST or IMAGE_USERFS_LIST with expected partition labels and mount point."
        exit 0
    fi

    # Generate images
    for part in ${IMAGE_USERFS_LIST}; do
        bbnote "Processing ${part}"
        image_userfs_partition_name=$(echo ${part} | cut -d':' -sf1)
        image_userfs_dir=$(echo ${part} | cut -d':' -sf2)
        image_userfs_name=$(echo ${part} | cut -d':' -sf3)
        image_userfs="${IMAGE_USERFS}/${image_userfs_partition_name}"

        [ -z "${image_userfs_partition_name}" ] && bbfatal "Configuration error: No partition entry found in ${part}"
        [ -z "${image_userfs_dir}" ] && bbfatal "Configuration error: No mountpoint entry found in ${part}"
        [ -z "${image_userfs_name}" ] && bbfatal "Configuration error: No package entry found in ${part}"

        bbnote "Partname  : ${image_userfs_partition_name}"
        bbnote "Mountpoint: ${image_userfs_dir}"
        bbnote "Package   : ${image_userfs_name}"
        bbnote "Workdir   : ${image_userfs}"

        if [ ! -d ${IMAGE_ROOTFS}${image_userfs_dir} ]; then
            if test "${IMAGE_USERFS_CREATE}" != "1"; then
                bberror "${image_userfs_partition_name} directory ${IMAGE_ROOTFS}${IMAGE_USERFS_DIR} does not exist"
                exit 0
            else
                mkdir -p "${IMAGE_ROOTFS}${image_userfs_dir}"
            fi
        fi

        if test -z "$(ls ${IMAGE_ROOTFS}${image_userfs_dir}/)"; then
            if test "${IMAGE_USERFS_CREATE}" != "1"; then
                bberror "${image_userfs_partition_name} directory ${IMAGE_ROOTFS}${image_userfs_dir} is empty"
                exit 0
            else
                touch "${IMAGE_ROOTFS}${image_userfs_dir}/dummy"
            fi
        fi

        mkdir ${image_userfs}
        cp -r ${IMAGE_ROOTFS}${image_userfs_dir}/* ${image_userfs}
        rm -rf ${IMAGE_ROOTFS}${image_userfs_dir}/*

        if [ -e ${IMGDEPLOYDIR}/${image_userfs_name}.tar.gz ];then
            rm -r ${IMGDEPLOYDIR}/${image_userfs_name}.tar.gz
        fi
        ( cd ${image_userfs}; tar cvfz ${IMGDEPLOYDIR}/${image_userfs_name}.tar.gz ./* )

    done

    # Here we generate the mount-userfs-partition.conf file
    # So it is not a part of the mount-userfs-partition package. The reason for that is, that
    # if it is generated in the package recipe, the image configuration is contained in this
    # package. But this is not valid! The image configuration belongs to the image recipe.
    # There might be cases where mount-userfs-partition is used in different images. So this
    # Recipe should have different contents, but the same version. This is also not handled
    # by yocto!

    # use a shell variable
    image_userfs_mount_list="${IMAGE_USERFS_MOUNT_LIST}"

    if [ -z "${image_userfs_mount_list}" ]; then
        # generate defaul list
        bbnote "IMAGE_USERFS_IMAGE_LIST is empty. Generate defaults from ${IMAGE_USERFS_LIST}"
        for part in ${IMAGE_USERFS_LIST}; do

            partname=$(echo ${part} | cut -d':' -sf1)
            mountpoint=$(echo ${part} | cut -d':' -sf2)
            archive=$(echo ${part} | cut -d':' -sf3)
            [ -z "${partname}" ] && bbfatal "Configuration error: No partition entry found in ${part}"
            [ -z "${mountpoint}" ] && bbfatal "Configuration error: No mountpoint entry found in ${part}"
            [ -z "${archive}" ] && bbfatal "Configuration error: No archive entry found in ${part}"
            image_userfs_mount_list="${image_userfs_mount_list}${partname},${mountpoint} "
            bbnote "partname ${partname}"
            bbnote "mountpoint ${mountpoint}"
            bbnote "archive ${archive}"
            bbnote "image_userfs_mount_list ${image_userfs_mount_list}"
        done
    fi

    # validate list
    for part in ${image_userfs_mount_list}; do
        partname=$(echo ${part} | cut -d',' -sf1)
        mountpoint=$(echo ${part} | cut -d',' -sf2)
        bbnote "mountpoint ${mountpoint}"
        [ -z "${partname}" ] && bbfatal "No partition entry found in ${part}"
        [ -z "${mountpoint}" ] && bbfatal "No mountpoint entry found in ${part}"
    done

    # write image config file
    echo "MOUNT_PARTITIONS_LIST=\"${image_userfs_mount_list}\"" > ${WORKDIR}/mount-userfs-partition.conf
    install -d ${IMAGE_ROOTFS}/${sysconfdir}

    # extend rootfs with config file
    install -m 644 ${WORKDIR}/mount-userfs-partition.conf ${IMAGE_ROOTFS}${sysconfdir}/
}

addtask userfs after do_rootfs before do_image_qa
do_userfs[depends] += "virtual/fakeroot-native:do_populate_sysroot"
do_userfs[cleandirs] = "${IMAGE_USERFS}"
