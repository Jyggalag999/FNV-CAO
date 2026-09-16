// Step 4 module port. Full QML replacement for ProgressWindow's internals - bound to a
// ProgressLogModel instance exposed as the "logModel" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: NebulaTheme.bgDeep

    NebulaDialogBackground {}

    Column {
        anchors.fill: parent
        anchors.margins: NebulaTheme.spacingM
        spacing: NebulaTheme.spacingS

        Row {
            spacing: NebulaTheme.spacingL

            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: NebulaTheme.textPrimary
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
            spacing: NebulaTheme.spacingS

            CaoButton { width: 100; text: "Clear log"; onClicked: logModel.clearLog() }
            CaoButton { width: 120; text: "Open log file"; onClicked: logModel.requestOpenLogFile() }
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
            height: NebulaTheme.controlHeight
            radius: NebulaTheme.radiusS
            color: NebulaTheme.bgInput
            border.width: 1
            border.color: NebulaTheme.borderColor

            Rectangle {
                width: logModel.progressMaximum > 0
                       ? parent.width * (logModel.progressValue / logModel.progressMaximum)
                       : 0
                height: parent.height
                radius: NebulaTheme.radiusS
                color: NebulaTheme.accentPrimary
                Behavior on width { NumberAnimation { duration: NebulaTheme.durationNormal } }
            }

            Text {
                anchors.centerIn: parent
                color: NebulaTheme.textPrimary
                text: logModel.progressText
            }
        }
    }
}
