// Shared hand-rolled integer spin box, matching CaoCheckBox/CaoTextField's conventions: never
// assigns to its own bound `value` property. Typing/stepper clicks only ever emit valueEdited()
// with the intended new value; the consumer decides whether/how to apply it to the bridge, and
// `value: bridge.someInt` binding flows the result back in - see CaoTextField.qml's comment for
// why this indirection matters (a self-mutated bound property's binding breaks permanently).
import QtQuick

Item {
    id: root

    property int value: 0
    property int from: 0
    property int to: 100
    property bool enabled: true

    signal valueEdited(int value)

    implicitWidth: 90
    implicitHeight: NebulaTheme.controlHeight

    function requestValue(v) {
        root.valueEdited(Math.max(from, Math.min(to, v)));
    }

    onValueChanged: {
        const parsed = parseInt(input.text, 10);
        if (isNaN(parsed) || parsed !== value)
            input.text = value.toString();
    }

    Rectangle {
        anchors.fill: parent
        radius: NebulaTheme.radiusS
        color: NebulaTheme.bgInput
        border.width: input.activeFocus ? 2 : 1
        border.color: input.activeFocus ? NebulaTheme.borderColorFocus : NebulaTheme.borderColor
        opacity: root.enabled ? 1.0 : 0.5
        Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }

        TextInput {
            id: input
            anchors.left: parent.left
            anchors.right: stepper.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6
            verticalAlignment: TextInput.AlignVCenter
            color: NebulaTheme.textPrimary
            selectionColor: NebulaTheme.accentHighlight
            selectedTextColor: NebulaTheme.bgDeep
            text: root.value.toString()
            enabled: root.enabled
            validator: IntValidator { bottom: root.from; top: root.to }

            onEditingFinished: root.requestValue(parseInt(text || "0", 10))
        }

        Column {
            id: stepper
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: 16

            Rectangle {
                width: 16
                height: 11
                color: upArea.containsMouse ? NebulaTheme.buttonTopHover : NebulaTheme.buttonTopNormal
                Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }

                Text {
                    anchors.centerIn: parent
                    color: NebulaTheme.textPrimary
                    font.pixelSize: 9
                    text: "+"
                }

                MouseArea {
                    id: upArea
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: root.enabled
                    onClicked: root.requestValue(root.value + 1)
                }
            }

            Rectangle {
                width: 16
                height: 11
                color: downArea.containsMouse ? NebulaTheme.buttonTopHover : NebulaTheme.buttonTopNormal
                Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }

                Text {
                    anchors.centerIn: parent
                    color: NebulaTheme.textPrimary
                    font.pixelSize: 9
                    text: "-"
                }

                MouseArea {
                    id: downArea
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: root.enabled
                    onClicked: root.requestValue(root.value - 1)
                }
            }
        }
    }
}
