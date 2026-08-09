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

        CaoGroupBox {
            width: parent.width
            title: "Process meshes"
            checkable: true
            checked: bridge.baseChecked
            onToggled: (checked) => bridge.baseChecked = checked

            Row {
                visible: bridge.baseChecked
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
}
