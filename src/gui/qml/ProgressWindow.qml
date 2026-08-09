// Step 4 module port. Full QML replacement for ProgressWindow's internals - bound to a
// ProgressLogModel instance exposed as the "logModel" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#0a0512"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Row {
            spacing: 16

            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: "#e6d8ef"
                text: "Log level:"
            }

            CaoRadioButton {
                anchors.verticalCenter: parent.verticalCenter
                checked: logModel.levelVerbose
                text: "Verbose"
                onClicked: logModel.selectLevelVerbose()
            }

            CaoRadioButton {
                anchors.verticalCenter: parent.verticalCenter
                checked: logModel.levelInfo
                text: "Info"
                onClicked: logModel.selectLevelInfo()
            }

            CaoRadioButton {
                anchors.verticalCenter: parent.verticalCenter
                checked: logModel.levelError
                text: "Error"
                onClicked: logModel.selectLevelError()
            }
        }

        Row {
            spacing: 8

            Rectangle {
                width: 100
                height: 28
                color: clearArea.pressed ? "#280028" : "#3c1450"

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    text: "Clear log"
                }

                MouseArea {
                    id: clearArea
                    anchors.fill: parent
                    onClicked: logModel.clearLog()
                }
            }

            Rectangle {
                width: 120
                height: 28
                color: openArea.pressed ? "#280028" : "#3c1450"

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    text: "Open log file"
                }

                MouseArea {
                    id: openArea
                    anchors.fill: parent
                    onClicked: logModel.requestOpenLogFile()
                }
            }
        }

        ListView {
            id: logView
            width: parent.width
            height: parent.height - 148
            clip: true
            spacing: 1
            model: logModel

            delegate: Text {
                width: logView.width
                wrapMode: Text.WordWrap
                visible: !model.hidden
                height: visible ? implicitHeight : 0
                color: model.color
                text: model.text
            }

            onCountChanged: positionViewAtEnd()
        }

        Rectangle {
            width: parent.width
            height: 24
            radius: 3
            color: "#170c26"
            border.color: "#4a2c6d"

            Rectangle {
                width: logModel.progressMaximum > 0
                       ? parent.width * (logModel.progressValue / logModel.progressMaximum)
                       : 0
                height: parent.height
                radius: 3
                color: "#642878"
            }

            Text {
                anchors.centerIn: parent
                color: "white"
                text: logModel.progressText
            }
        }
    }
}
