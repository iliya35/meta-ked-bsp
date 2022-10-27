TEMPLATE = app
TARGET = webengine-vk
QT += qml quick gui webengine widgets
SOURCES += main.cpp\
           createHTML.cpp
HEADERS += createHTML.h

RESOURCES += \
    qml.qrc

target.path = /opt/webengine-vk
INSTALLS += target
