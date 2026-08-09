// Shared hand-rolled radio button, matching CaoCheckBox's palette/conventions. Exclusivity
// between sibling radio buttons is the consuming module's responsibility (bind each one's
// `checked` off a shared bridge property) - this component stays dumb/stateless like CaoCheckBox.
import QtQuick

Item {
    id: root

    property bool checked: false
    property string text: ""

    signal clicked()

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    Row {
        id: row
        spacing: 6

        Rectangle {
            width: 16
            height: 16
            radius: 8
            anchors.verticalCenter: parent.verticalCenter
            color: "#170c26"
            border.color: "#4a2c6d"

            Rectangle {
                visible: root.checked
                anchors.centerIn: parent
                width: 10
                height: 10
                radius: 5
                color: "#d98fe0"
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            color: "#e6d8ef"
            text: root.text
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
