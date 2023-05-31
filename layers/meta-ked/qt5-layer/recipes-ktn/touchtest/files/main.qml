import QtQuick 2.3
import QtQuick.Controls 1.2

ApplicationWindow {
    visible: true
    title: qsTr("Hello World")

    StackView
    {
        id: stack
        //property int  selected : 0
        //property var  selIng : []
        property bool firstBusTest: false
        property bool firstBusTestOK: false
        anchors.fill: parent
        initialItem: Qt.resolvedUrl("touchtest.qml")
        delegate: StackViewDelegate
        {
            pushTransition: StackViewTransition{}
            popTransition:  StackViewTransition{}
        }
    }
}
