import QtQuick 2.4
import QtQuick.Window 2.2
import QtWebEngine 1.1

Window {
    visible: true
    WebEngineView {
            id: webview
            url: (Qt.application.arguments[1] == undefined ? "http://www.google.de":Qt.application.arguments[1])
            anchors.fill: parent
    }
}
