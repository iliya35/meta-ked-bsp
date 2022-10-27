SUMMARY = "An image with additional tests for KED hardware"

require recipes-core/images/image-ktn.bb

IMAGE_EXTRA_INSTALL += " \
	alsa-utils-alsamixer \
	alsa-utils-amixer \
	alsa-utils-aplay \
	alsa-utils-speakertest \
	devmem2 \
	glmark2 \
	htop \
	i2c-tools \
	iotop \
	iperf3 \
	iptables \
	iputils-ping \
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
	"
