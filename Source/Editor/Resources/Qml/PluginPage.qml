import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

RowLayout{
    anchors.fill: parent
    Rectangle{
        Layout.fillHeight: true
        width : 300
        Layout.verticalStretchFactor : 5
        Layout.margins: 10
        RectangularGlow {
            anchors.fill: parent
            glowRadius: 5
            spread: 0.2
            color: "#DDDDDD"
            cornerRadius:  glowRadius
        }
        Rectangle {
            color: "white"
            anchors.fill: parent
            radius: 4
        }
        TreeView {
            id: outliner
            model: pluginsModel
            anchors.fill: parent
            property var currentPlugin : null
            delegate: Item {
                id: treeDelegate

                implicitWidth: outliner.width
                implicitHeight: 50

                property bool hoverd : false
                property color hoverdColor : "transparent"
                // Assigned to by TreeView:
                required property TreeView treeView
                required property bool isTreeNode
                required property bool expanded
                required property int hasChildren
                required property int depth

                Rectangle {
                    visible: model == outliner.currentPlugin
                    anchors.fill: parent
                    color:  echoxStyle.SelectedColor
                }

                Rectangle {
                    anchors.fill: parent
                    color:  hoverdColor
                }
                RowLayout{
                    spacing: 10
                    anchors.fill: parent
                    anchors.margins: 5
                    Rectangle{
                        width:  40
                        height:  40
                        Image {
                            id: icon
                            source: model.icon
                            sourceSize.width:  40
                            sourceSize.height:   40
                        }
                        ColorOverlay {
                                anchors.fill: icon
                                source: icon
                                color: echoxStyle.ThemeColor
                        }
                    }
                    Text {
                        Layout.fillWidth: true
                        text: model.name
                        color: "#444444"
                        Layout.alignment: Qt.AlignLeft
                    }
                }
                MouseArea{
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
                        if(outliner.currentPlugin != model){
                            outliner.currentPlugin = model
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
    }
    ColumnLayout{
        Layout.fillWidth: true
        spacing: 15
        Rectangle{
            Layout.fillWidth : true
            Layout.fillHeight: true
            Layout.margins: 10
            Layout.verticalStretchFactor : 5
            RectangularGlow {
                anchors.fill: parent
                glowRadius: 5
                spread: 0.2
                color: "#DDDDDD"
                cornerRadius:  glowRadius
            }
            Rectangle {
                color: "white"
                anchors.fill: parent
                radius: 4
            }
            SwipeView {
                id: view
                clip: true
                anchors.fill: parent
                Repeater {
                    model: outliner.currentPlugin ? Math.max(1,outliner.currentPlugin.images.length) : 0
                    Loader {
                        active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                        sourceComponent: Image {
                            anchors.fill: parent
                            source: outliner.currentPlugin.images.length === 0 ? outliner.currentPlugin.icon: outliner.currentPlugin.images[index]
                        }
                    }
                }
            }

            PageIndicator {
                id: indicator

                count: view.count
                currentIndex: view.currentIndex

                anchors.bottom: view.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        RowLayout{
            spacing: 10
            Layout.fillHeight: true
            Layout.verticalStretchFactor : 1
            Text {
                text: qsTr("Name:")
                color : "#444444"
                Layout.leftMargin:  20
                Layout.minimumWidth: 250
            }
            TextEdit{
                readOnly: true
                color : "#666666"
                Layout.fillWidth: true
                text: outliner.currentPlugin ? outliner.currentPlugin.name :""
            }
        }
        RowLayout{
            spacing: 10
            Layout.fillHeight: true
            Layout.verticalStretchFactor : 1
            Text {
                text: qsTr("Author:")
                color : "#444444"
                Layout.leftMargin:  20
                Layout.minimumWidth : 250
            }
            TextEdit{
                readOnly: true
                color : "#666666"
                Layout.fillWidth: true
                text: outliner.currentPlugin ? outliner.currentPlugin.author : ""
            }
        }
        RowLayout{
            spacing: 10
            Layout.fillHeight: true
            Layout.verticalStretchFactor : 1
            Text {
                text: qsTr("Link:")
                color : "#444444"
                Layout.leftMargin:  20
                Layout.minimumWidth : 250
            }
            TextEdit{
                readOnly: true
                color : "#666666"
                Layout.fillWidth: true
                text: outliner.currentPlugin ? outliner.currentPlugin.link : ""
            }
        }
        Text {
            text: "Desc:"
            color : "#444444"
            Layout.leftMargin:  20
        }
        Rectangle{
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor : 5
            Layout.minimumHeight: 50
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.bottomMargin: 20
            color: "#F1F1F1"
            TextEdit {
                color : "#666666"
                readOnly: true
                anchors.fill: parent
                anchors.margins: 10
                text: outliner.currentPlugin ? outliner.currentPlugin.desc : ""
            }
        }

    }
}
