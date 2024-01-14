import QtQuick 
import QtQuick.Window 
import QtQuick.Controls 
import Qt.labs.platform 
import QWindowKit

Window {
    id: window
    width: 1200
    height: 800
    color: darkStyle.windowBackgroundColor
    title: qsTr("EchoX")
    Component.onCompleted: {
        //windowAgent.setup(window)
        window.visible = true
    }

    WindowAgent {
        id: windowAgent
    }

    Rectangle {
        id: titleBar
        anchors {
            top: parent.top
            topMargin: 1
            left: parent.left
            right: parent.right
        }
        height: 32
        color: window.active ? "#3C3C3C" : "#505050"
        //Component.onCompleted: windowAgent.setTitleBar(titleBar)

        IconButton {
            id: goBackButton
            visible: stackView.depth > 1
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 2
            }
            height: parent.height
            source: "qrc:///Resources/Icon/back.svg"
            onClicked: Controller.goBack()
            //Component.onCompleted: windowAgent.setSystemButton(WindowAgent.Minimize, minButton)
        }

        Text {
            id: pageName
            anchors {
                verticalCenter: parent.verticalCenter
                left: goBackButton.visible? goBackButton.right: parent.left
                leftMargin: 10
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: window.title
            font.pixelSize: 14
            color: "#ECECEC"
        }

        Row {
            anchors {
                top: parent.top
                right: parent.right
            }
            height: parent.height

            IconButton {
                id: settingsButton
                visible: !(stackView.currentItem && stackView.currentItem.objectName === "Settings")
                height: parent.height
                source: "qrc:///Resources/Icon/setting-fill.svg"
                onClicked: Controller.openSettingsPage()
                //Component.onCompleted: windowAgent.setSystemButton(WindowAgent.Minimize, minButton)
            }

            IconButton {
                id: minButton
                height: parent.height
                source: "qrc:///Resources/Icon/minimize.svg"
                onClicked: window.showMinimized()
                //Component.onCompleted: windowAgent.setSystemButton(WindowAgent.Minimize, minButton)
            }

            IconButton {
                id: maxButton
                height: parent.height
                source: window.visibility === Window.Maximized ? "qrc:///Resources/Icon/restore.svg" : "qrc:///Resources/Icon/maximize.svg"
                onClicked: {
                    if (window.visibility === Window.Maximized) {
                        window.showNormal()
                    } else {
                        window.showMaximized()
                    }
                }
                //Component.onCompleted: windowAgent.setSystemButton(WindowAgent.Maximize, maxButton)
            }

            IconButton {
                id: closeButton
                height: parent.height
                source: "qrc:///Resources/Icon/close.svg"
                background: Rectangle {
                    color: {
                        if (!closeButton.enabled) {
                            return "gray";
                        }
                        if (closeButton.pressed) {
                            return "#e81123";
                        }
                        if (closeButton.hovered) {
                            return "#e81123";
                        }
                        return "transparent";
                    }
                }
                onClicked: window.close()
                //Component.onCompleted: windowAgent.setSystemButton(WindowAgent.Close, closeButton)
            }
        }
    }

    StackView {
        id: stackView
        initialItem: mainView
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        pushEnter: Transition {
             PropertyAnimation {
                 property: "opacity"
                 from: 0
                 to:1
                 duration: 400
             }
         }
         pushExit: Transition {
             PropertyAnimation {
                 property: "opacity"
                 from: 1
                 to:0
                 duration: 400
             }
         }
         popEnter: Transition {
             PropertyAnimation {
                 property: "opacity"
                 from: 0
                 to:1
                 duration: 400
             }
         }
         popExit: Transition {
             PropertyAnimation {
                 property: "opacity"
                 from: 1
                 to:0
                 duration: 400
             }
         }

        Component.onCompleted:{
            Controller.initialize(pageName,stackView)
        }
    }
    
    Component {
        id: mainView
        ProjectsPage{
            objectName : "EchoX"
        }
    }
}
