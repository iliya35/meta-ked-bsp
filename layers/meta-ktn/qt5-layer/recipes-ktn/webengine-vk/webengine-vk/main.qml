import QtQuick 2.0
import QtQuick.VirtualKeyboard 1.0
import QtQuick.Window 2.0
import QtWebEngine 1.1
import QtQuick.Controls 1.0

Rectangle
{
    id: root
    color: "white"
    property int addBarHeigth: Qt.application.arguments.indexOf("showBar") !== -1 ? 40 : 0
    transform: Rotation{angle:rot ? rota : 0;origin.x: width/2; origin.y: height/2}
    Item
    {
        id: appContainer
        width:  (rota === 0 || rota === 180) ? parent.width : parent.height
        height: (rota === 0 || rota === 180) ? parent.height : parent.width
        anchors.centerIn: parent
        Keys.onPressed:
        {
            if (event.key === Qt.Key_F5)
            {
                console.log("key F5 pressed, refreshing page");
                webview.reload();
                event.accepted = true;
            }
        }
        Rectangle
        {
            id: addBar
            anchors{top: parent.top;left: parent.left;right: parent.right}
            height: addBarHeigth
            TextField
            {
                id: tf
                anchors.left: parent.left
                height: addBarHeigth
                width: parent.width-2*height
                text: indexPath
                onTextChanged: if(text.substring(text.length-1,text.length) === "\n") but.load()
                onAccepted: but.load()
            }
            Text
            {
                id: urltxt
                visible: false
                text: indexPath
            }
            Button
            {
                id: refreshBut
                anchors{left: tf.right;right: but.left}
                height: addBarHeigth; width: addBarHeigth
                onClicked: webview.reload()
                Image
                {
                    source: "refresh_google_materials.svg"
                    sourceSize{width: parent.height;height: parent.width}
                }
            }
            Button
            {
                id: but
                anchors{right: parent.right}
                height: addBarHeigth; width: addBarHeigth
                text: "OK"
                onClicked: load()
                function load()
                {
                    Qt.inputMethod.hide()
                    if (tf.text.substring(0,7) !== "http://" && tf.text.substring(0,7) !== "file://" && tf.text.substring(0,8) !== "https://")
                        tf.text = "http://" + tf.text
                    tf.text = tf.text.trim()
                    urltxt.text = tf.text
                }
            }
        }
        WebEngineView
        {
            id: webview
            url: urltxt.text
            anchors{top:addBar.bottom;left:parent.left;right:parent.right;bottom:inputPanel.top}
            antialiasing: true
            onCertificateError: error.ignoreCertificateError();
            onNewViewRequested: request.openIn(webview2)
            onUrlChanged: tf.text = url
        }
        WebEngineView
        {
            id: webview2
            width: 0
            height: 0
            onUrlChanged:
            {
                createHTML.getURL(webview2.url.toString());
                webview.url= htmlPath + "out.htm"
            }
        }
        InputPanel
        {
            id: inputPanel
            z: 99
            y: appContainer.height
            anchors.left: parent.left
            anchors.right: parent.right
            states: State
            {
                name: "visible"
                when: Qt.inputMethod.visible
                PropertyChanges
                {
                    target: inputPanel
                    y: appContainer.height - inputPanel.height
                }
            }
            transitions: Transition
            {
                from: ""
                to: "visible"
                reversible: true
                ParallelAnimation
                {
                    NumberAnimation
                    {
                        properties: "y"
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
    }
}
