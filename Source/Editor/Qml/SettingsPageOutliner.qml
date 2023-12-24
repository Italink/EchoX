import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TreeView {
    objectName: "hello"
    id: outliner
    model: settingsModel
    Layout.fillWidth : true
    Layout.fillHeight: true
    editTriggers : TableView.DoubleTapped

    property var currentSettings : null

    Connections{
        target:settingsModel
        function onSettingsChanged(){
            outliner.expandRecursively(-1,-1)
        }
    }

    Component.onCompleted: {
        outliner.expandRecursively(-1,-1)
    }

    delegate: Item {
        id: treeDelegate
        implicitWidth: outliner.width
        implicitHeight: label.implicitHeight * 1.5

        property bool hoverd : false
        property color hoverdColor : "transparent"

        readonly property real indent: 20
        readonly property real padding: 5

        // Assigned to by TreeView:
        required property TreeView treeView
        required property bool isTreeNode
        required property bool expanded
        required property int hasChildren
        required property int depth
        Rectangle {
            visible: treeDelegate.treeView.index(row, column) == currentSettings 
            anchors.fill: parent
            color:  echoxStyle.SelectedColor
        }
        Rectangle {
            anchors.fill: parent
            color:  hoverdColor
        }
        Text {
            id: label
            x: padding + (treeDelegate.isTreeNode ? (treeDelegate.depth + 1) * treeDelegate.indent : 0)
            y: implicitHeight*0.25
            width: treeDelegate.width - treeDelegate.padding - x
            text: model.name
            color: treeDelegate.hasChildren ? "#444444" : "#666666"
            font.pointSize: treeDelegate.hasChildren ? 16 : 8
            anchors.topMargin: 10
            anchors.bottomMargin: 10
        }
        MouseArea{
            enabled: !treeDelegate.hasChildren
            hoverEnabled : true
            anchors.fill: parent
            onEntered:{
                hoverd = true
                exitAnimation.stop()
                enterAnimation.start()
            }
            onExited:{
                hoverd = false
                enterAnimation.stop()
                exitAnimation.start()
            }
            onClicked: {
                var modelIndex = treeDelegate.treeView.index(row, column)
                if(outliner.currentSettings != modelIndex){
                    outliner.currentSettings = modelIndex
                    settingsModel.notifyCurrentSettingChanged(modelIndex)
                }
            }
        }
        ColorAnimation on hoverdColor{
            id: enterAnimation
            to: echoxStyle.HoveredColor
            duration: 100
            running: false
        }
        ColorAnimation on hoverdColor{
            id: exitAnimation
            to: "transparent"
            duration: 100
            running: false
        }
    }
}
