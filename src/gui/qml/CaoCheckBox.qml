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
    property bool enabled: true

    signal toggled(bool checked)

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    activeFocusOnTab: root.enabled
    opacity: root.enabled ? 1.0 : 0.6

    Keys.onReturnPressed: if (root.enabled) root.toggled(!root.checked)
    Keys.onSpacePressed: if (root.enabled) root.toggled(!root.checked)

    Row {
        id: row
        spacing: NebulaTheme.spacingS

        Rectangle {
            width: NebulaTheme.indicatorSize
            height: NebulaTheme.indicatorSize
            radius: NebulaTheme.radiusS - 2
            anchors.verticalCenter: parent.verticalCenter
            color: NebulaTheme.bgInput
            // Pink border (not the blue-purple every other control uses) paired with a blue
            // checked-fill below - focus still wins out to cyan regardless, same as everywhere
            // else, so keyboard focus never gets ambiguous with any other state/color scheme.
            border.width: root.activeFocus ? 2 : 1
            border.color: root.activeFocus
                          ? NebulaTheme.borderColorFocus
                          : (area.containsMouse ? NebulaTheme.borderColorAccentStrong : NebulaTheme.borderColorAccent)
            Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }

            Rectangle {
                anchors.centerIn: parent
                width: 10
                height: 10
                radius: 1
                color: NebulaTheme.accentBlue
                opacity: root.checked ? 1.0 : 0.0
                scale: root.checked ? 1.0 : 0.6
                Behavior on opacity { NumberAnimation { duration: NebulaTheme.durationFast } }
                Behavior on scale { NumberAnimation { duration: NebulaTheme.durationFast; easing.type: Easing.OutBack } }
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            color: NebulaTheme.textPrimary
            text: root.text
        }
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        enabled: root.enabled
        onClicked: {
            root.forceActiveFocus();
            root.toggled(!root.checked);
        }
    }
}
