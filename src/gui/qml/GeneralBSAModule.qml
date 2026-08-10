// Step 4 module port. Full QML replacement for GeneralBSAModule's internals - bound to a
// GeneralBSABridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    id: root
    anchors.fill: parent
    color: "#170c26"

    // QML-local toggle for the "More" box's own visibility. No corresponding Settings field exists
    // for this - it's a new UI-only control (not present in the original Widgets app) added per
    // request to let "More" collapse independently of "Process BSAs".
    property bool moreChecked: true

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 16

        CaoGroupBox {
            width: parent.width
            title: "Process BSAs"
            checkable: true
            checked: bridge.baseChecked
            onToggled: (checked) => bridge.baseChecked = checked

            Row {
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

        CaoGroupBox {
            width: parent.width
            title: "More"
            checkable: true
            checked: root.moreChecked
            onToggled: (checked) => root.moreChecked = checked

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
