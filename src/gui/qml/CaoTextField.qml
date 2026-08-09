// Shared hand-rolled single-line text field, matching CaoCheckBox's conventions: never assigns
// to its own bound `text` property (the inner TextInput's own `text` absorbs keystrokes locally,
// which is unavoidable - the user needs to see what they type - but `root.text` itself stays
// externally driven). `root.text`'s binding therefore never breaks; instead, its onTextChanged
// explicitly re-syncs the inner TextInput whenever the bridge-driven value changes from outside,
// which is what actually needs to happen once the *inner* TextInput's own binding to root.text
// has been broken by the user's first keystroke (same fundamental issue CaoCheckBox has, just
// unavoidable at the TextInput leaf instead of avoidable by not self-mutating at all).
import QtQuick

Item {
    id: root

    property string text: ""
    property string placeholderText: ""

    signal textEdited(string text)

    implicitWidth: 220
    implicitHeight: 24

    onTextChanged: {
        if (input.text !== text)
            input.text = text;
    }

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
            text: root.text
            onTextEdited: root.textEdited(text)
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
