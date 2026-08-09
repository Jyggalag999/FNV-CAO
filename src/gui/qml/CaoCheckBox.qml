// Shared hand-rolled checkbox, matching the app's existing dark palette (see
// MainWindow.cpp's get_dark_style_sheet() for the QSS equivalent this mirrors). No
// QtQuick.Controls dependency - this app's bespoke theming would need as much custom styling
// work on top of Controls' Basic style as hand-rolling does, so there's little to gain from it
// yet. Revisit if/when that stops being true.
//
// Deliberately never assigns to its own `checked` property (a click only emits toggled() with
// the intended new value) - every consumer binds `checked: bridge.someBool` declaratively, and a
// QML binding is permanently destroyed the instant anything assigns to the bound property
// imperatively. Self-mutating on click would sever that binding after the first click, so later
// bridge-driven changes (switching profiles, etc.) would silently stop updating this checkbox.
import QtQuick

Item {
    id: root

    property bool checked: false
    property string text: ""

    signal toggled(bool checked)

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    Row {
        id: row
        spacing: 6

        Rectangle {
            width: 16
            height: 16
            radius: 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#170c26"
            border.color: "#4a2c6d"

            Rectangle {
                visible: root.checked
                anchors.centerIn: parent
                width: 10
                height: 10
                radius: 1
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
        onClicked: root.toggled(!root.checked)
    }
}
