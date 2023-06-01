SUMMARY = "The Qt5 Yocto image for the Kontron Electronics hardware"

require recipes-core/images/image-ked.bb

inherit populate_sdk_qt5

########################################
#########     DEMOS    #################
########################################
IMAGE_INSTALL += "			\
	kontron-demo			\
	kontron-demo-autostart		\
	kontron-demo-video		\
	kontron-demo-slideshow		\
	animatedtiles			\
	imagegestures			\
	cinematicexperience		\
	webengine-vk			\
	glmark2				\
	qtserialbus			\
	libsocketcan-dev		\
"
