// Step 4 module port. Full QML replacement for AdvancedAnimationsModule's internals - bound to
// an AdvancedAnimationsBridge instance exposed as the "bridge" context property.
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
            title: "Base"

            CaoCheckBox {
                checked: bridge.necessaryOpt
                text: "Necessary optimization"
                onToggled: (checked) => bridge.necessaryOpt = checked
            }
        }
    }
}
