SUMMARY = "An image with additional tests for KED hardware"

require recipes-core/images/image-ktn.bb

IMAGE_EXTRA_INSTALL += " \
	alsa-utils-alsamixer \
	alsa-utils-amixer \
	alsa-utils-aplay \
	alsa-utils-speakertest \
	devmem2 \
	glmark2 \
	gstreamer1.0 \
	gstreamer1.0-plugins-bad \
	gstreamer1.0-plugins-base \
	gstreamer1.0-plugins-good \
	htop \
	i2c-tools \
	iotop \
	iperf3 \
	iptables \
	iputils-ping \
	libcamera \
	libdrm-tests \
	libegl-mesa \
	libgles2-mesa \
	lmbench \
	memtester \
	mtd-utils-tests \
	nano \
	neard \
	net-tools \
	pciutils \
	spidev-test \
	spitools \
	stress \
	stress-ng \
	sysbench \
	v4l-utils \
	"
