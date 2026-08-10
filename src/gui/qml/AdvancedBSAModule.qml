// Step 4 module port. Full QML replacement for AdvancedBSAModule's internals - bound to an
// AdvancedBSABridge instance exposed as the "bridge" context property.
//
// Root is a plain (transparent) Item filling the tab page, not the visible purple box itself -
// see the Rectangle below, sized to its own content (matching CaoGroupBox.qml's technique)
// rather than stretching to fill the tab page's full height, which left a large empty stretch of
// solid color below the content. See GeneralBSAModule.qml for the full rationale.
import QtQuick

Item {
    anchors.fill: parent

    Rectangle {
        width: parent.width
        height: column.implicitHeight + 24
        color: "#170c26"

        Column {
            id: column
            x: 12
            y: 12
            width: parent.width - 24
            spacing: 8

            CaoGroupBox {
                width: parent.width
                title: "File setting"

                CaoCheckBox {
                    checked: bridge.packFile
                    text: "Pack file"
                    onToggled: (checked) => bridge.packFile = checked
                }
            }
        }
    }
}
