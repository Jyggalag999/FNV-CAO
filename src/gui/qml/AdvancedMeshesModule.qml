// Step 4 module port. Full QML replacement for AdvancedMeshesModule's internals - bound to an
// AdvancedMeshesBridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#170c26"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        CaoCheckBox {
            checked: bridge.baseChecked
            text: "Process meshes"
            onToggled: (checked) => bridge.baseChecked = checked
        }

        Row {
            visible: bridge.baseChecked
            leftPadding: 20
            spacing: 24

            CaoRadioButton {
                checked: !bridge.fullOptimization
                text: "Recommended"
                onClicked: bridge.fullOptimization = false
            }

            CaoRadioButton {
                checked: bridge.fullOptimization
                text: "Extensive"
                onClicked: bridge.fullOptimization = true
            }
        }
    }
}
