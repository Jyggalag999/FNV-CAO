// Shared hand-rolled button. Translucent, matching every other panel/surface in the app (see
// NebulaTheme.qml's withAlpha()) rather than a flat gradient or a photo crop - both tried here
// first, both replaced per feedback: the image version looked good but was one more thing
// visually competing for attention on every button in the app; translucency keeps buttons
// consistent with the rest of the "see the nebula through it" language everything else already
// uses. Brighter on hover, darker on press (opacity shift, not a color swap - a plain fill can't
// itself change hue the way a gradient could, so alpha is what animates), cyan focus outline,
// dimmed disabled state. Colors/radius/spacing/timing all come from NebulaTheme rather than
// literal hex values.
//
// mainGroupBox's native Run/Open Directory QPushButtons use the same translucent language (see
// MainWindow.cpp's QPushButton#processButton/#userPathButton rules) plus a pink accent-colored
// label to mark them out as the app's two primary actions - this component keeps plain
// textPrimary, since every other button using it (Manage, dialog actions) is a secondary action,
// not a primary one; giving all of them the same pink treatment would erase that distinction
// rather than extend it.
import QtQuick

Item {
    id: root

    property string text: ""
    property bool enabled: true

    signal clicked()

    implicitWidth: label.implicitWidth + NebulaTheme.spacingXL
    implicitHeight: NebulaTheme.controlHeight

    activeFocusOnTab: root.enabled

    Keys.onReturnPressed: if (root.enabled) root.clicked()
    Keys.onSpacePressed: if (root.enabled) root.clicked()

    Rectangle {
        anchors.fill: parent
        radius: NebulaTheme.radiusS
        opacity: root.enabled ? 1.0 : 0.6
        // More translucent than the general panel alpha (was 0.6/0.45/0.85) - Manage/dialog
        // buttons are small and numerous, so keeping them this see-through matters more for the
        // nebula reading as the actual focus of the UI than it does for the larger panels.
        color: NebulaTheme.withAlpha(NebulaTheme.bgElevated,
                                     area.pressed ? 0.6 : (area.containsMouse ? 0.25 : 0.35))
        Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }

        border.width: root.activeFocus ? 2 : 1
        border.color: root.activeFocus
                      ? NebulaTheme.borderColorFocus
                      : (area.containsMouse && !area.pressed ? NebulaTheme.borderColorStrong : NebulaTheme.borderColor)
        Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }

        Text {
            id: label
            anchors.centerIn: parent
            color: root.enabled ? NebulaTheme.textPrimary : NebulaTheme.textDisabled
            text: root.text
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
}
