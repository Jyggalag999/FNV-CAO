// Shared hand-rolled single-line text field, matching CaoCheckBox/CaoRadioButton's conventions.
// `text` is an alias straight onto the inner TextInput's own text property, not a separate
// manually-synced one - avoids the usual binding-loop pitfall of hand-rolled text fields (an
// external `text: bridge.someString` binding breaks cleanly on the first keystroke, same as any
// plain TextInput bound to a model, no manual sync needed).
import QtQuick

Item {
    id: root

    property alias text: input.text
    property string placeholderText: ""

    signal editingFinished()

    implicitWidth: 220
    implicitHeight: 24

    Rectangle {
        anchors.fill: parent
        radius: 3
        color: "#170c26"
        border.color: input.activeFocus ? "#2d8ae0" : "#4a2c6d"

        TextInput {
            id: input
            anchors.fill: parent
            anchors.margins: 6
            verticalAlignment: TextInput.AlignVCenter
            color: "#e6d8ef"
            selectionColor: "#2d5aa5"
            clip: true
            onEditingFinished: root.editingFinished()
        }

        Text {
            visible: input.text.length === 0
            anchors.fill: input
            verticalAlignment: Text.AlignVCenter
            color: "#6a5a76"
            text: root.placeholderText
        }
    }
}
