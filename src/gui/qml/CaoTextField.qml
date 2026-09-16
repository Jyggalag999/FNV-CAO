// Shared hand-rolled single-line text field, matching CaoCheckBox's conventions: never assigns
// to its own bound `text` property (the inner TextInput's own `text` absorbs keystrokes locally,
// which is unavoidable - the user needs to see what they type - but `root.text` itself stays
// externally driven). `root.text`'s binding therefore never breaks; instead, its onTextChanged
// explicitly re-syncs the inner TextInput whenever the bridge-driven value changes from outside,
// which is what actually needs to happen once the *inner* TextInput's own binding to root.text
// has been broken by the user's first keystroke (same fundamental issue CaoCheckBox has, just
// unavoidable at the TextInput leaf instead of avoidable by not self-mutating at all).
//
// Background is NebulaTheme.bgInput, darker than the panels this normally sits in (CaoGroupBox's
// content box, etc.) - inputs should read as a well sunk into the surrounding panel, not another
// panel of their own; see the design spec's Text Inputs section.
import QtQuick

Item {
    id: root

    property string text: ""
    property string placeholderText: ""

    signal textEdited(string text)

    implicitWidth: 220
    implicitHeight: NebulaTheme.controlHeight

    onTextChanged: {
        if (input.text !== text)
            input.text = text;
    }

    Rectangle {
        anchors.fill: parent
        radius: NebulaTheme.radiusS
        color: NebulaTheme.bgInput
        border.width: input.activeFocus ? 2 : 1
        border.color: input.activeFocus ? NebulaTheme.borderColorFocus : NebulaTheme.borderColor
        Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }

        TextInput {
            id: input
            anchors.fill: parent
            anchors.margins: 6
            verticalAlignment: TextInput.AlignVCenter
            color: NebulaTheme.textPrimary
            selectionColor: NebulaTheme.accentHighlight
            selectedTextColor: NebulaTheme.bgDeep
            clip: true
            text: root.text
            onTextEdited: root.textEdited(text)
        }

        Text {
            visible: input.text.length === 0
            anchors.fill: input
            verticalAlignment: Text.AlignVCenter
            color: NebulaTheme.textDisabled
            text: root.placeholderText
        }
    }
}
