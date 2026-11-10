// Step 4 module port. Full QML replacement for ProgressWindow's internals - bound to a
// ProgressLogModel instance exposed as the "logModel" context property.
import QtQuick
import QtQuick.Controls

Rectangle {
    anchors.fill: parent
    color: NebulaTheme.bgDeep

    NebulaDialogBackground {}

    // Header rows at the top, progress bar pinned to the bottom, log fills what's between (was a
    // Column with the log at a hardcoded parent.height - 148, which left a gap under the bar at
    // 100% and would overlap/clip at other scales).
    Item {
        anchors.fill: parent
        anchors.margins: NebulaTheme.spacingM

        Column {
            id: header
            width: parent.width
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
        }

        ListView {
            id: logView
            anchors.top: header.bottom
            anchors.topMargin: NebulaTheme.spacingS
            anchors.bottom: progressBar.top
            anchors.bottomMargin: NebulaTheme.spacingS
            width: parent.width
            clip: true
            ScrollBar.vertical: ScrollBar { policy: size < 1.0 ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff }
            spacing: 1
            model: logModel

            delegate: Text {
                width: logView.width - NebulaTheme.spacingM // room for the scrollbar
                wrapMode: Text.WordWrap
                visible: !model.hidden
                height: visible ? implicitHeight : 0
                color: model.color
                text: model.text
            }

            // Deferred: wrapped delegates only get their final height after layout, so jumping to
            // the end immediately left the last line half below the bottom edge.
            onCountChanged: Qt.callLater(positionViewAtEnd)
        }

        Rectangle {
            id: progressBar
            anchors.bottom: parent.bottom
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
                width: Math.min(implicitWidth, parent.width - NebulaTheme.spacingM)
                elide: Text.ElideMiddle // long file paths would spill past the bar
                horizontalAlignment: Text.AlignHCenter
                color: NebulaTheme.textPrimary
                text: logModel.progressText
            }
        }
    }
}
