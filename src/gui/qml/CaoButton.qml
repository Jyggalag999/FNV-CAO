// Shared hand-rolled button, matching the app's real QPushButtons (see MainWindow.cpp's
// get_dark_style_sheet() QPushButton rules, which this mirrors): a vertical gradient fill,
// purple border, brighter hover gradient/border, flat pressed fill, dimmed disabled state.
import QtQuick

Item {
    id: root

    property string text: ""
    property bool enabled: true

    signal clicked()

    implicitWidth: label.implicitWidth + 24
    implicitHeight: 26

    Rectangle {
        anchors.fill: parent
        radius: 4
        opacity: root.enabled ? 1.0 : 0.6
        border.width: 1
        border.color: area.containsMouse && !area.pressed ? "#642878" : "#4a2c6d"

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: area.pressed ? "#962d00" : (area.containsMouse ? "#50145a" : "#3c1450")
            }
            GradientStop {
                position: 1.0
                color: area.pressed ? "#962d00" : (area.containsMouse ? "#3c0032" : "#280028")
            }
        }

        Text {
            id: label
            anchors.centerIn: parent
            color: "#f0e0f5"
            text: root.text
        }

        MouseArea {
            id: area
            anchors.fill: parent
            hoverEnabled: true
            enabled: root.enabled
            onClicked: root.clicked()
        }
    }
}
