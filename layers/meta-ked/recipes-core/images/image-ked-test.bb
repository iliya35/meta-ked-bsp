SUMMARY = "An image with additional tests for KED hardware"

inherit image-configuration

SRC_URI += "file://configuration/"

require recipes-core/images/image-ked.bb

PACKAGES_BASIC = "\
	bmap-tools \
	devmem2 \
	fb-test \
	gdb \
	gdbserver \
	htop \
	i2c-tools \
	iotop \
	iperf3 \
	iptables \
	iputils-ping \
	lmbench \
	memtester \
	mmc-utils \
	mtd-utils-tests \
	nano \
	neard \
	net-tools \
	pciutils \
	spidev-test \
	spitools \
	strace \
	stress \
	stress-ng \
	sysbench \
	"

PACKAGES_MULTIMEDIA = "\
	alsa-utils-alsamixer \
	alsa-utils-amixer \
	alsa-utils-aplay \
	alsa-utils-speakertest \
	glmark2 \
	gstreamer1.0 \
	gstreamer1.0-plugins-bad \
	gstreamer1.0-plugins-base \
	gstreamer1.0-plugins-good \
	libcamera \
	libdrm-tests \
	libegl-mesa \
	libgles2-mesa \
	v4l-utils \
	"

IMAGE_EXTRA_INSTALL = "\
	${PACKAGES_BASIC} \
	${PACKAGES_MULTIMEDIA} \
	${PACKAGES_PYTHON} \
	"
