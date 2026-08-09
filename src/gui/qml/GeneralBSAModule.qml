// Step 4 module port. Full QML replacement for GeneralBSAModule's internals - bound to a
// GeneralBSABridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#170c26"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 16

        Column {
            spacing: 8

            CaoCheckBox {
                checked: bridge.baseChecked
                text: "Process BSAs"
                onToggled: (checked) => bridge.baseChecked = checked
            }

            Row {
                visible: bridge.baseChecked
                leftPadding: 20
                spacing: 24

                CaoRadioButton {
                    checked: bridge.extractMode
                    text: "Extract BSAs"
                    onClicked: bridge.extractMode = true
                }

                CaoRadioButton {
                    checked: !bridge.extractMode
                    text: "Create BSAs"
                    onClicked: bridge.extractMode = false
                }
            }
        }

        Column {
            spacing: 8

            Text {
                color: "#d98fe0"
                font.pixelSize: 13
                text: "More"
            }

            CaoCheckBox {
                checked: bridge.dontMakeLoaded
                text: "Do not make BSA loaded"
                onToggled: (checked) => bridge.dontMakeLoaded = checked
            }

            CaoCheckBox {
                checked: bridge.dontCompress
                text: "Do not compress files"
                onToggled: (checked) => bridge.dontCompress = checked
            }

            CaoCheckBox {
                checked: bridge.dontRemoveFiles
                text: "Do not remove packed files"
                onToggled: (checked) => bridge.dontRemoveFiles = checked
            }

            CaoCheckBox {
                visible: bridge.makeOverridesVisible
                checked: bridge.makeOverrides
                text: "Create override files"
                onToggled: (checked) => bridge.makeOverrides = checked
            }

            Row {
                spacing: 8

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#e6d8ef"
                    text: "Archive name:"
                }

                CaoTextField {
                    anchors.verticalCenter: parent.verticalCenter
                    text: bridge.archiveName
                    placeholderText: "Leave blank for automatic naming"
                    onTextEdited: (text) => bridge.archiveName = text
                }
            }
        }
    }
}
