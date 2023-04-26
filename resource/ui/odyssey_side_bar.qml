import QtQuick
import QtQuick.Controls

Rectangle {
    id: sideBar
    property int sideBarWidth: 48
    property color hoveredColor: "white"
    property color unhoveredColor: "gray"

    width: sideBarWidth
    height: 559
    visible: true
    color: "#24292E"
    state: ""

    Button {
        id: typeButton
        width: sideBarWidth
        height: sideBarWidth
        flat: true
        icon.width: sideBarWidth
        icon.height: sideBarWidth
        icon.source: "qrc:/image/type.png"
        icon.color: hovered ? hoveredColor : unhoveredColor
        state: ""
        MouseArea {
            anchors.fill: parent
            onClicked: typeButton.state == "clicked" ? sideBar.state = "" : sideBar.state = "typeButton"
        }
        background: Rectangle {
            color: "transparent"
        }
        states: [
            State {
                name: "clicked"
                PropertyChanges {
                    target: typeButton
                    icon.color: hoveredColor
                }
            }
        ]
    }

    Button {
        id: softButton
        anchors.left: typeButton.left
        anchors.top: typeButton.bottom
        width: sideBarWidth
        height: sideBarWidth
        flat: true
        icon.width: sideBarWidth
        icon.height: sideBarWidth
        icon.source: "qrc:/image/soft.png"
        icon.color: hovered ? hoveredColor : unhoveredColor
        state: ""
        MouseArea {
            anchors.fill: parent
            onClicked: softButton.state == "clicked" ? sideBar.state = "" : sideBar.state = "softButton"
        }
        background: Rectangle {
            color: "transparent"
        }
        states: [
            State {
                name: "clicked"
                PropertyChanges {
                    target: softButton
                    icon.color: hoveredColor
                }
            }
        ]
    }

    Button {
        id: hardButton
        anchors.left: softButton.left
        anchors.top: softButton.bottom
        width: sideBarWidth
        height: sideBarWidth
        flat: true
        icon.width: sideBarWidth
        icon.height: sideBarWidth
        icon.source: "qrc:/image/hard.png"
        icon.color: hovered ? hoveredColor : unhoveredColor
        state: ""
        MouseArea {
            anchors.fill: parent
            onClicked: hardButton.state == "clicked" ? sideBar.state = "" : sideBar.state = "hardButton"
        }
        background: Rectangle {
            color: "transparent"
        }
        states: [
            State {
                name: "clicked"
                PropertyChanges {
                    target: hardButton
                    icon.color: hoveredColor
                }
            }
        ]
    }

    states: [
        State {
            name: "typeButton"
            PropertyChanges {
                target: typeButton
                state: "clicked"
            }
            PropertyChanges {
                target: softButton
                state: ""
            }
            PropertyChanges {
                target: hardButton
                state: ""
            }
        },
        State {
            name: "softButton"
            PropertyChanges {
                target: typeButton
                state: ""
            }
            PropertyChanges {
                target: softButton
                state: "clicked"
            }
            PropertyChanges {
                target: hardButton
                state: ""
            }
        },
        State {
            name: "hardButton"
            PropertyChanges {
                target: typeButton
                state: ""
            }
            PropertyChanges {
                target: softButton
                state: ""
            }
            PropertyChanges {
                target: hardButton
                state: "clicked"
            }
        }
    ]
}
