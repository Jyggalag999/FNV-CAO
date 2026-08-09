// Step 4 module port. Full QML replacement for LevelSelector's internals - bound to a
// LevelSelectorBridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#0a0512"

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Row {
            spacing: 40

            Repeater {
                model: [
                    { text: "Quick Optimize", hover: () => bridge.hoverQuickOptimize(), select: () => bridge.selectQuickOptimize() },
                    { text: "Medium", hover: () => bridge.hoverMedium(), select: () => bridge.selectMedium() },
                    { text: "Advanced", hover: () => bridge.hoverAdvanced(), select: () => bridge.selectAdvanced() },
                ]

                delegate: Rectangle {
                    width: 140
                    height: 32
                    color: modeArea.pressed ? "#280028" : (modeArea.containsMouse ? "#50145a" : "#3c1450")

                    Text {
                        anchors.centerIn: parent
                        color: "white"
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
            color: "#e6d8ef"
            text: bridge.helpText
        }

        CaoCheckBox {
            checked: bridge.rememberChoice
            text: "Remember my choice"
            onToggled: (checked) => bridge.rememberChoice = checked
        }
    }
}
