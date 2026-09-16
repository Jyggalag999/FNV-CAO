// Step 4 module port. Full QML replacement for LevelSelector's internals - bound to a
// LevelSelectorBridge instance exposed as the "bridge" context property.
//
// The three mode buttons stay a custom Rectangle rather than CaoButton - they need an
// onEntered-triggered hover callback (bridge.hoverX(), used to preview helpText before a
// selection is made) that CaoButton has no signal for, not just a differently-shaped button.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: NebulaTheme.bgDeep

    NebulaDialogBackground {}

    Column {
        anchors.fill: parent
        anchors.margins: NebulaTheme.spacingL
        spacing: NebulaTheme.spacingL

        Row {
            spacing: NebulaTheme.spacingXL

            Repeater {
                model: [
                    { text: "Quick Optimize", hover: () => bridge.hoverQuickOptimize(), select: () => bridge.selectQuickOptimize() },
                    { text: "Medium", hover: () => bridge.hoverMedium(), select: () => bridge.selectMedium() },
                    { text: "Advanced", hover: () => bridge.hoverAdvanced(), select: () => bridge.selectAdvanced() },
                ]

                delegate: Rectangle {
                    width: 140
                    height: 32
                    radius: NebulaTheme.radiusS
                    border.width: 1
                    border.color: modeArea.containsMouse ? NebulaTheme.borderColorStrong : NebulaTheme.borderColor
                    Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }

                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: modeArea.pressed
                                   ? NebulaTheme.buttonTopPressed
                                   : (modeArea.containsMouse ? NebulaTheme.buttonTopHover : NebulaTheme.buttonTopNormal)
                            Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }
                        }
                        GradientStop {
                            position: 1.0
                            color: modeArea.pressed
                                   ? NebulaTheme.buttonBottomPressed
                                   : (modeArea.containsMouse ? NebulaTheme.buttonBottomHover : NebulaTheme.buttonBottomNormal)
                            Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        color: NebulaTheme.textPrimary
                        text: modelData.text
                    }

                    MouseArea {
                        id: modeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: modelData.hover()
                        onClicked: modelData.select()
                    }
                }
            }
        }

        Text {
            width: parent.width
            wrapMode: Text.WordWrap
            color: NebulaTheme.textPrimary
            text: bridge.helpText
        }

        CaoCheckBox {
            checked: bridge.rememberChoice
            text: "Remember my choice"
            onToggled: (checked) => bridge.rememberChoice = checked
        }
    }
}
