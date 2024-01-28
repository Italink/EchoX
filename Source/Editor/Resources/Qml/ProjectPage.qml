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
            clip: true
            id: componentOutliner
            model: ComponentModel
            anchors.fill: parent
            delegate: Item {
                implicitWidth: componentOutliner.width
                implicitHeight: 50

                property bool hoverd : false
                property color hoverdColor : "transparent"
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
                        Layout.alignment: Qt.AlignLeft |Qt.AlignVCenter
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
                    onPositionChanged: (mouse)=>{
                        if(mouse.buttons & Qt.LeftButton){
                            ComponentModel.notifyBeginDrag(index)
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
    SplitView{
        id: splitView
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: 10
        orientation: Qt.Vertical
        handle: Rectangle {
             id: handleDelegate
             implicitWidth: 4
             implicitHeight: 4
             color: SplitHandle.pressed ? EchoXStyle.HoveredColor
                 : (SplitHandle.hovered ? Qt.lighter(EchoXStyle.HoveredColor, 1.1) : EchoXStyle.HoveredColor)
     }
        Rectangle{
            SplitView.minimumHeight: 200
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
                id: projectOutliner
                model: CurrentProjectModel
                clip: true
                anchors.fill: parent
                property var currComponent : null
                delegate: Item {
                    implicitWidth: projectOutliner.width
                    implicitHeight: 30
                    property bool hoverd : false
                    property color hoverdColor : "transparent"
                    Rectangle {
                        visible: model == projectOutliner.currComponent
                        anchors.fill: parent
                        color:  EchoXStyle.SelectedColor
                    }

                    Rectangle {
                        anchors.fill: parent
                        color:  hoverdColor
                    }
                    Text {
                        anchors.fill: parent
                        text: model.name
                        color: "#444444"
                        verticalAlignment:  Text.AlignVCenter
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
                            if(projectOutliner.currComponent != model){
                                projectOutliner.currComponent = model
                                var comp = CurrentProjectModel.getComponentByIndex(index)
                                detailsView.setObject(comp)
                                console.log(comp)
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
        }
        Rectangle{
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
            SplitView.minimumHeight: 400
            DetailsView {
                clip: true
                id: detailsView
                anchors.fill: parent
            }
        }
    }
}
