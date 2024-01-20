import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Qt.DetailsView

RowLayout{
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
        ListView{
            id: componentOutliner
            model: PluginsModel
            anchors.fill: parent
            property var currentPlugin : null
            delegate: Item {


                implicitWidth: componentOutliner.width
                implicitHeight: 50

                property bool hoverd : false
                property color hoverdColor : "transparent"
                Rectangle {
                    visible: model == componentOutliner.currentPlugin
                    anchors.fill: parent
                    color:  EchoXStyle.SelectedColor
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
                                color: EchoXStyle.ThemeColor
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
                        if(componentOutliner.currentPlugin != model){
                            componentOutliner.currentPlugin = model
                        }
                    }
                }
                ColorAnimation on hoverdColor{
                    id: enterAnimation
                    to: EchoXStyle.HoveredColor
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
        Layout.fillHeight: true
        spacing: 15
        ListView{
            id: projectOutliner
            model: PluginsModel
            Layout.fillWidth: true
            Layout.fillHeight: true
            property var currentPlugin : null
            delegate: Item {
                implicitWidth: projectOutliner.width
                implicitHeight: 50

                property bool hoverd : false
                property color hoverdColor : "transparent"
                Rectangle {
                    visible: model == projectOutliner.currentPlugin
                    anchors.fill: parent
                    color:  EchoXStyle.SelectedColor
                }

                Rectangle {
                    anchors.fill: parent
                    color:  hoverdColor
                }

                MouseArea{
                    hoverEnabled : true
                    anchors.fill: parent
                    onEntered:{
                        hoverd = true
                        projectExitAnimation.stop()
                        projectEnterAnimation.start()
                    }
                    onExited:{
                        hoverd = false
                        projectEnterAnimation.stop()
                        projectExitAnimation.start()
                    }
                    onClicked: {
                        if(projectOutliner.currentPlugin != model){
                            projectOutliner.currentPlugin = model
                        }
                    }
                }
                ColorAnimation on hoverdColor{
                    id: projectEnterAnimation
                    to: EchoXStyle.HoveredColor
                    duration: 100
                    running: false
                }
                ColorAnimation on hoverdColor{
                    id: projectExitAnimation
                    to: "transparent"
                    duration: 100
                    running: false
                }

            }
        }
        DetailsView {
            Component.onCompleted: {
                setObject(projectEnterAnimation)
            }
        }
    }
}
