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

    signal valueEdited(int value)

    implicitWidth: 90
    implicitHeight: 24

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
        radius: 3
        color: "#170c26"
        border.color: input.activeFocus ? "#2d8ae0" : "#4a2c6d"
        opacity: root.enabled ? 1.0 : 0.5

        TextInput {
            id: input
            anchors.left: parent.left
            anchors.right: stepper.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6
            verticalAlignment: TextInput.AlignVCenter
            color: "#e6d8ef"
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
                color: "#3c1450"

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: 9
                    text: "+"
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: root.enabled
                    onClicked: root.requestValue(root.value + 1)
                }
            }

            Rectangle {
                width: 16
                height: 11
                color: "#3c1450"

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: 9
                    text: "-"
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: root.enabled
                    onClicked: root.requestValue(root.value - 1)
                }
            }
        }
    }
}
