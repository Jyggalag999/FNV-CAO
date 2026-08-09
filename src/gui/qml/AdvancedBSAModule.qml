// Step 4 module port. Full QML replacement for AdvancedBSAModule's internals - bound to an
// AdvancedBSABridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#170c26"

    Column {
        anchors.fill: parent
        anchors.margins: 12
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
