TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp

RESOURCES += qml.qrc

TARGET = touchtest

target.path = /opt/$${TARGET}/

INSTALLS += target
