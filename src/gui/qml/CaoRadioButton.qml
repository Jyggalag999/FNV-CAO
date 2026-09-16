// Shared hand-rolled radio button, matching CaoCheckBox's palette/conventions/theme tokens.
// Exclusivity between sibling radio buttons is the consuming module's responsibility (bind each
// one's `checked` off a shared bridge property) - this component stays dumb/stateless like
// CaoCheckBox.
import QtQuick

Item {
    id: root

    property bool checked: false
    property string text: ""
    property bool enabled: true

    signal clicked()

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    activeFocusOnTab: root.enabled
    opacity: root.enabled ? 1.0 : 0.6

    Keys.onReturnPressed: if (root.enabled) root.clicked()
    Keys.onSpacePressed: if (root.enabled) root.clicked()

    Row {
        id: row
        spacing: NebulaTheme.spacingS

        Rectangle {
            width: NebulaTheme.indicatorSize
            height: NebulaTheme.indicatorSize
            radius: width / 2
            anchors.verticalCenter: parent.verticalCenter
            color: NebulaTheme.bgInput
            border.width: root.activeFocus ? 2 : 1
            border.color: root.activeFocus
                          ? NebulaTheme.borderColorFocus
                          : (area.containsMouse ? NebulaTheme.borderColorStrong : NebulaTheme.borderColor)
            Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }

            Rectangle {
                anchors.centerIn: parent
                width: 10
                height: 10
                radius: 5
                color: NebulaTheme.accentPrimary
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
            root.clicked();
        }
    }
}
