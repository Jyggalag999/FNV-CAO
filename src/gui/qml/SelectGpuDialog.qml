// Step 3 pattern-proof module. Full QML replacement for SelectGPUWindow's internals (see
// SelectGPUWindow.hpp/.cpp) - bound to a SelectGpuBridge instance exposed as the "bridge" context
// property. No QtQuick.Controls dependency yet (deliberately - this module is meant to be small
// and self-contained); hand-rolled rows and buttons match the app's existing dark palette.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#0a0512"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text {
            color: "#f0d2e1"
            font.pixelSize: 14
            text: "GPU"
        }

        Repeater {
            model: bridge.deviceNames

            delegate: Rectangle {
                width: 376
                height: 32
                color: index === bridge.selectedIndex ? "#642878" : "#170c26"
                border.color: "#4a2c6d"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    color: "#e6d8ef"
                    text: modelData
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: bridge.selectedIndex = index
                }
            }
        }

        Row {
            anchors.right: parent.right
            spacing: 8

            Rectangle {
                width: 80
                height: 28
                color: cancelArea.pressed ? "#280028" : "#3c1450"

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    text: "Cancel"
                }

                MouseArea {
                    id: cancelArea
                    anchors.fill: parent
                    onClicked: bridge.reject()
                }
            }

            Rectangle {
                width: 80
                height: 28
                color: okArea.pressed ? "#3c1450" : "#642878"

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    text: "OK"
                }

                MouseArea {
                    id: okArea
                    anchors.fill: parent
                    onClicked: bridge.accept()
                }
            }
        }
    }
}
