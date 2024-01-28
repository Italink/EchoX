import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

ColumnLayout{

    RowLayout{
        Layout.fillWidth: true
        Layout.fillHeight: true

        Text {
            text: qsTr("Order:")
            color : "#444444"
            Layout.leftMargin:  20
            Layout.minimumWidth: 150
        }
        ButtonGroup{
            buttons: order.children
        }
        Row{
            id : order
            padding : 20
            spacing : 20
            RadioButton{
                text: qsTr("Name")
                palette.buttonText : "#666666"
            }
            RadioButton{
                text: qsTr("Create Time")
                palette.buttonText : "#666666"
            }
            RadioButton{
                text: qsTr("Modify Time")
                palette.buttonText : "#666666"
            }
        }
        Button{
            text: qsTr("Create New Project")
            onClicked: {
                Controller.createNewProject()
            }
        }
    }

    GridView {
        id: projectsView
        property real scaleFactor: 1
        model: ProjectsModel
        Layout.fillWidth: true
        Layout.fillHeight: true
        cellWidth  : 100 * scaleFactor
        cellHeight : 100 * scaleFactor + 50
        Keys.enabled: true
        focus: true
        clip: true
        MouseArea{
            id: viewMouseArea
            visible: false
            anchors.fill: parent
            cursorShape: Qt.SizeAllCursor
            propagateComposedEvents: true
            onWheel: (wheel)=>{
                if (wheel.modifiers & Qt.ControlModifier){
                    if (wheel.angleDelta.y > 0)
                    {
                        projectsView. scaleFactor = Math.min(projectsView.scaleFactor + 0.1, 5)
                    }
                    else
                    {
                        projectsView.scaleFactor = Math.max(projectsView.scaleFactor - 0.1,0.5)
                    }
                    wheel.accepted = true
                }
            }
            onPositionChanged:(mouse)=>{
                mouse.accepted = false
            }
        }
        Keys.onPressed: (event)=> {
            if (event.key == Qt.Key_Control) {
                viewMouseArea.visible = true
            }
        }
        Keys.onReleased : (event)=> {
            if (event.key == Qt.Key_Control) {
                viewMouseArea.visible = false
            }
        }

        delegate: Rectangle {
            property color hoverdColor : "transparent"
            width: projectsView.cellWidth
            height: projectsView.cellHeight
            color:  hoverdColor
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
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: icon.bottom
                anchors.bottom: parent.bottom
                text: model.name
            }
            MouseArea{
                hoverEnabled : true
                anchors.fill: parent
                propagateComposedEvents: true
                cursorShape: Qt.PointingHandCursor
                onEntered:{
                    exitAnimation.stop()
                    enterAnimation.start()
                }
                onExited:{
                    enterAnimation.stop()
                    exitAnimation.start()
                }
                onDoubleClicked: {
                    console.log(index)
                    ProjectsModel.notifyProjectDoubleClicked(index)
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
