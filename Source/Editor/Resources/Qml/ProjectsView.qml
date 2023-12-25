import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

GridView {
    id: projectsView
    model: ["apple", "banan asdasdasda asdasd asd asd as asd asd asd asd", "che  asd rryasd asdasas", "dasasd atasde asdas","appl a sa e", "banana", "cherry", "date","apple", "banana", "cherry", "date","apple", "banana", "cherry", "date"]
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
            horizontalAlignment: Text.AlignHCenter
            wrapMode:Text.WordWrap
            anchors.left: icon.left
            anchors.right: icon.right
            anchors.top: icon.bottom
            anchors.bottom: parent.bottom
            text: modelData
        }
    }
}
