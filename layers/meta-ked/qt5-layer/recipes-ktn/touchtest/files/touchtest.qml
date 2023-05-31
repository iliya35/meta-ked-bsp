import QtQuick 2.0
import QtQuick.Window 2.0

Window
{
    id: root
    visible: true
    property int bins_horz : Qt.application.arguments.length > 2 ? Qt.application.arguments[1] : 12
    property int binz_vert : Qt.application.arguments.length > 2 ? Qt.application.arguments[2] : 5
    Rectangle
    {
        anchors.fill: parent
        color: "red"
        Text
        {
            anchors.centerIn: parent
            color: "white"
            text: rep.coloredRect ? "" : "Touch-Test\nBitte mit Finger über den Bildschirm fahren\nund einzelne Sektoren berühren."
        }
        Repeater
        {
            id: rep
            property int coloredRect : 0
            model: bins_horz * binz_vert
            Rectangle
            {
                property bool colorchanged: false
                x: Math.round((index % bins_horz) * root.width / bins_horz)
                y: Math.floor(index/bins_horz) * root.height / binz_vert
                width: root.width/bins_horz
                height: root.height/binz_vert
                color: "transparent"
            }
            onColoredRectChanged: if(rep.coloredRect === (bins_horz*binz_vert)) timer.start();
        }
        MultiPointTouchArea
        {
            anchors.fill: parent
            onPressed: color(touchPoints)
            onUpdated: color(touchPoints)
            onReleased: color(touchPoints)
            function color(tp)
            {
                for(var i = 0; i < tp.length; i++)
                {
                    var point = tp[i];
                    var x_ind = Math.floor(point.x/root.width*bins_horz)
                    var y_ind = Math.floor(point.y*binz_vert/root.height);
                    var ind = x_ind + bins_horz * y_ind;
                    if(!rep.itemAt(ind).colorchanged)
                    {
                        rep.itemAt(ind).color = "green";
                        rep.itemAt(ind).colorchanged = true;
                        rep.coloredRect++
                    }
                }
            }
        }
        Timer
        {
            id: timer
            running: false; repeat: false; interval: 1000
            onTriggered: Qt.quit()
        }
    }
}
