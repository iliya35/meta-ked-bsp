inherit userfs-settings

# install mount script in image
IMAGE_INSTALL += "mount-userfs-partition"

parse_userfs_list() {
    bbnote "Processing ${1}"
    image_userfs_partition_name=$(echo ${1} | cut -d':' -sf1)
    image_userfs_dir=$(echo ${1} | cut -d':' -sf2)
    image_userfs_name=$(echo ${1} | cut -d':' -sf3)
    image_userfs_size=$(echo ${1} | cut -d':' -sf4)
    image_userfs="${IMAGE_USERFS}${image_userfs_partition_name}"
    image_userfs_src="${IMAGE_ROOTFS}${image_userfs_dir}"

    [ -z "${image_userfs_partition_name}" ] && \
        bbfatal "Configuration error: No partition entry found in '${1}'"
    [ -z "${image_userfs_dir}" ] && \
        bbfatal "Configuration error: No mountpoint entry found in '${1}'"
    [ -z "${image_userfs_name}" ] && \
        bbfatal "Configuration error: No package entry found in '${1}'"

    if [ "$(echo ${IMAGE_USERFS_TYPES} | grep -w ext4)" != "" ]; then
        [ -z "${image_userfs_size}" ] && \
            bbfatal "Configuration error: No image size set in '${1}'"
    fi

    bbnote "Partname  : ${image_userfs_partition_name}"
    bbnote "Mountpoint: ${image_userfs_dir}"
    bbnote "Package   : ${image_userfs_name}"
    bbnote "Workdir   : ${image_userfs}"
}

fakeroot do_userfs() {
    if [ ${IMAGE_CREATE_USERFS} -eq 0 ]; then
        exit 0
    fi

    # Check basic configuration
    if [ -z "${IMAGE_USERFS_MOUNT_LIST}" -a -z "${IMAGE_USERFS_LIST}" ]; then
        bbfatal "Please set either IMAGE_USERFS_MOUNT_LIST or IMAGE_USERFS_LIST with expected partition labels and mount point."
    fi

    # Copy userfs files
    for userfs in ${IMAGE_USERFS_LIST}; do
        parse_userfs_list $userfs

        if [ ! -d ${image_userfs_src} ]; then
            if test "${IMAGE_USERFS_CREATE}" != "1"; then
                bberror "${image_userfs_partition_name} directory ${image_userfs_src} does not exist"
                exit 0
            else
                mkdir -p "${image_userfs_src}"
            fi
        fi

        if test -z "$(ls ${image_userfs_src}/)"; then
            if test "${IMAGE_USERFS_CREATE}" != "1"; then
                bberror "${image_userfs_partition_name} directory ${image_userfs_src} is empty"
                exit 0
            else
                touch "${image_userfs_src}/dummy"
            fi
        fi

        mkdir ${image_userfs}
        cp -r ${IMAGE_ROOTFS}${image_userfs_dir}/* ${image_userfs}
        rm -rf ${IMAGE_ROOTFS}${image_userfs_dir}/*
    done
}

do_image_userfs() {
    for userfs in ${IMAGE_USERFS_LIST}; do
        parse_userfs_list $userfs

        if [ -e ${IMGDEPLOYDIR}/${image_userfs_name}.tar.gz ]; then
            rm -r ${IMGDEPLOYDIR}/${image_userfs_name}.tar.gz
        fi
        if [ -e ${IMGDEPLOYDIR}/${image_userfs_name}.ext4 ]; then
            rm -r ${IMGDEPLOYDIR}/${image_userfs_name}.ext4
        fi
        (
            cd ${image_userfs};
            for type in "${IMAGE_USERFS_TYPES}"; do
                case $type in
                tar.gz)
                    tar cvfz ${IMGDEPLOYDIR}/${image_userfs_name}.tar.gz ./*
                    ;;
                ext4)
                    # If generating an empty image the size of the sparse block
                    # should be large enough to allocate an ext4 filesystem
                    # using 4096 bytes per inode, this is about 60K, so dd needs
                    # a minimum count of 60, with bs=1024 (bytes per IO)
                    eval local COUNT=\"0\"
                    eval local MIN_COUNT=\"60\"
                    if [ ${image_userfs_size} -lt $MIN_COUNT ]; then
                        eval COUNT=\"$MIN_COUNT\"
                    fi
                    dd if=/dev/zero of=${IMGDEPLOYDIR}/${image_userfs_name}.ext4 seek=${image_userfs_size} count=$COUNT bs=1024
                    mkfs.ext4 ${IMGDEPLOYDIR}/${image_userfs_name}.ext4 -d .
                    # Error codes 0-3 indicate successfull operation of fsck
                    # (no errors or errors corrected)
                    fsck.ext4 -pvfD ${IMGDEPLOYDIR}/${image_userfs_name}.ext4 || [ $? -le 3 ]
                    ;;
                esac
            done
        )
    done
}

do_userfs_mounts() {
    # use a shell variable
    image_userfs_mount_list="${IMAGE_USERFS_MOUNT_LIST}"

    if [ -z "${image_userfs_mount_list}" ]; then
        # generate default list
        bbnote "IMAGE_USERFS_IMAGE_LIST is empty. Generate defaults from \${IMAGE_USERFS_LIST}"
        for userfs in ${IMAGE_USERFS_LIST}; do
            parse_userfs_list $userfs
            image_userfs_mount_list="${image_userfs_mount_list} ${image_userfs_partition_name},${image_userfs_dir}"
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
    echo "MOUNT_PARTITIONS_LIST=\"${image_userfs_mount_list}\"" > ${WORKDIR}${IMAGE_USERFS_MOUNT_CONF}
    install -d ${IMAGE_ROOTFS}/${sysconfdir}

    # extend rootfs with config file
    install -m 644 ${WORKDIR}${IMAGE_USERFS_MOUNT_CONF} ${IMAGE_ROOTFS}${sysconfdir}/
}

addtask userfs after do_rootfs before do_image_qa
addtask image_userfs after do_userfs before do_image_qa
addtask userfs_mounts after do_image_userfs before do_image_qa
do_userfs[depends] += "virtual/fakeroot-native:do_populate_sysroot"
do_image_userfs[depends] += "e2fsprogs-native:do_populate_sysroot"
do_userfs[cleandirs] = "${IMAGE_USERFS}"
