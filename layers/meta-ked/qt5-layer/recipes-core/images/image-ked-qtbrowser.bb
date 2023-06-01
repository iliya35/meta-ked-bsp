SUMMARY = "A demo image including QtWebBrowser"

require recipes-core/images/image-ked.bb

inherit populate_sdk_qt5
inherit image-configuration

SRC_URI = " \
	file://autostart-app;subdir=${IMAGE_CONFIGFILES_INSTALL}/etc/default/	\
	file://ntp.conf;subdir=${IMAGE_CONFIGFILES_INSTALL}/etc/				\
"

IMAGE_INSTALL += "			\
	autostart-app			\
	qtwebbrowser			\
	ntp						\
	tzdata					\
"
