// Step 3 pattern-proof module. Full QML replacement for SelectGPUWindow's internals (see
// SelectGPUWindow.hpp/.cpp) - bound to a SelectGpuBridge instance exposed as the "bridge" context
// property. No QtQuick.Controls dependency yet (deliberately - this module is meant to be small
// and self-contained); hand-rolled rows share NebulaTheme with the rest of the app, and Cancel/OK
// use the shared CaoButton rather than one-off Rectangles.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: NebulaTheme.bgDeep

    NebulaDialogBackground {}

    Column {
        anchors.fill: parent
        anchors.margins: NebulaTheme.spacingM
        spacing: NebulaTheme.spacingS

        Text {
            color: NebulaTheme.textPrimary
            font.pixelSize: 14
            font.weight: Font.DemiBold
            text: "GPU"
        }

        Repeater {
            model: bridge.deviceNames

            delegate: Rectangle {
                width: 376
                height: 32
                radius: NebulaTheme.radiusS
                color: index === bridge.selectedIndex
                       ? NebulaTheme.withAlpha(NebulaTheme.accentPrimary, 0.35)
                       : (rowArea.containsMouse ? NebulaTheme.rowHover : NebulaTheme.withAlpha(NebulaTheme.bgPanel, NebulaTheme.panelAlpha))
                border.width: 1
                border.color: index === bridge.selectedIndex ? NebulaTheme.accentPrimary : NebulaTheme.borderColor
                Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }
                Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: NebulaTheme.spacingS
                    color: NebulaTheme.textPrimary
                    text: modelData
                }

                MouseArea {
                    id: rowArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: bridge.selectedIndex = index
                }
            }
        }

        Row {
            anchors.right: parent.right
            spacing: NebulaTheme.spacingS

            CaoButton { width: 80; text: "Cancel"; onClicked: bridge.reject() }
            CaoButton { width: 80; text: "OK"; onClicked: bridge.accept() }
        }
    }
}
