import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

GridView {
    id: projectsView
    model: ["apple", "banan asdasdasda", "che rryasdas", "dasasd atasde asdas","apple", "banana", "cherry", "date","apple", "banana", "cherry", "date","apple", "banana", "cherry", "date"]
    cellWidth  : 100
    cellHeight : 150
    delegate: Rectangle {
        width: projectsView.cellWidth
        height: projectsView.cellHeight
        Rectangle{
            id: icon
            anchors.fill: parent
            anchors.margins: 10
            anchors.bottomMargin: parent.height - parent.width + 10
            color: "red"
        }
        Text {
            elide: Text.ElideRight
            wrapMode:Text.WordWrap
            anchors.fill: parent
            anchors.top: icon.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: modelData
        }
    }
}
