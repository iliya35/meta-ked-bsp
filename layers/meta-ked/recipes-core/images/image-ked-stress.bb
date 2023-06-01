SUMMARY = "An image for stress testing KED hardware"

require recipes-core/images/image-ked.bb

IMAGE_EXTRA_INSTALL += " \
	devmem2 \
	glmark2 \
	htop \
	i2c-tools \
	iotop \
	iperf3 \
	iptables \
	iputils-ping \
	libdrm-tests \
	lmbench \
	memtester \
	mtd-utils-tests \
	nano \
	neard \
	net-tools \
	pciutils \
	stress \
	stress-ng \
	sysbench \
	ktnstress-autostart \
	"
