SUMMARY = "An image with additional tests for KED hardware"

require recipes-core/images/image-ked.bb

PACKAGES_BASIC = "\
	bmap-tools \
	devmem2 \
	fbida \
	fb-test \
	gdb \
	gdbserver \
	i2c-tools \
	iotop \
	iperf3 \
	iptables \
	iputils-ping \
	lmbench \
	memtester \
	mmc-utils \
	mtd-utils-tests \
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

PACKAGES_CONVENIENCE = "\
	htop \
	nano \
	screen \
	tmux \
	tree \
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

PACKAGES_PYTHON = "\
	libgpiod-python \
	python3 \
	python3-can \
	python3-numpy \
	python3-pip \
	python3-pymodbus \
	python3-pyserial \
	python3-setuptools \
"

IMAGE_EXTRA_INSTALL = "\
	${PACKAGES_BASIC} \
	${PACKAGES_CONVENIENCE} \
	${PACKAGES_PYTHON} \
	${PACKAGES_MULTIMEDIA} \
	"
