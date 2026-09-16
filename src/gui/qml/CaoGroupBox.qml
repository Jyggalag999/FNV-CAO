// Shared hand-rolled group box, matching the app's existing dark palette and the visual role of
// the old Widgets QGroupBox (see e.g. GeneralBSAModule's "Process BSAs"/"More" groups,
// AdvancedMeshesModule's "Process meshes" group). Supports the two shapes the old .ui files used:
//   - plain (checkable: false) - a static title, e.g. "More", "Base", "File setting".
//   - checkable (checkable: true) - the title doubles as a checkbox, e.g. "Process BSAs",
//     "Process meshes", "Process textures", "Resizing" - same as a checkable QGroupBox.
// Checkable mode never assigns to its own `checked` property (see CaoCheckBox.qml's comment for
// why) - it only emits toggled() with the intended value.
//
// One simplification from the original: title sits above the border rather than inset into the
// border line (the classic QGroupBox look) - avoids needing to know the parent's background color
// to mask the border where an inset label would overlap it.
//
// Checkable mode collapses the bordered content area entirely when unchecked (not just dims it) -
// but the title row (which is the only way to re-check it) always stays visible, so unchecking
// never hides the control needed to check it back on.
//
// Content area is a plain, unboxed Item now - no fill, no border. Was a translucent Rectangle
// (bgElevated - see NebulaTheme.qml); removed per feedback that the stacked boxes (this one,
// GeneralBSAModule.qml's own contentBox around it, QGroupBox/QTabWidget::pane further out) made
// the UI read as "boxes on boxes" competing with the actual nebula backdrop they all sit over.
// Individual controls (CaoCheckBox, CaoTextField, etc.) keep their own small, functional
// boundaries - it's specifically the big structural wrapper rectangles that are gone.
import QtQuick

Item {
    id: root

    property string title: ""
    property bool checkable: false
    property bool checked: true

    // Whether the bordered content area is showing right now.
    readonly property bool contentVisible: !root.checkable || root.checked

    default property alias content: contentColumn.children

    signal toggled(bool checked)

    implicitWidth: contentColumn.implicitWidth + NebulaTheme.spacingL
    implicitHeight: titleRow.implicitHeight
                    + (root.contentVisible ? NebulaTheme.spacingS + contentColumn.implicitHeight + NebulaTheme.spacingL : 0)

    Row {
        id: titleRow
        spacing: NebulaTheme.spacingS

        // Same blue-fill/pink-border scheme as CaoCheckBox.qml, not the purple every border
        // elsewhere uses - this indicator is functionally a checkbox (it's what makes "Process
        // BSAs" etc. checkable), so it should read as one, matching its plain-CaoCheckBox
        // siblings rather than looking like a different control.
        Rectangle {
            visible: root.checkable
            width: NebulaTheme.indicatorSize
            height: NebulaTheme.indicatorSize
            radius: NebulaTheme.radiusS - 2
            anchors.verticalCenter: parent.verticalCenter
            color: NebulaTheme.bgInput
            border.width: 1
            border.color: titleArea.containsMouse ? NebulaTheme.borderColorAccentStrong : NebulaTheme.borderColorAccent
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

        // Blue instead of purple - purple text over this app's purple/magenta nebula backdrop
        // was low-contrast and hard to read; blue (the nebula's own cooler accent - see
        // NebulaTheme.qml's accentBlue) stands out from it instead of blending in. This is the
        // one shared component behind every tab's section titles ("Process BSAs", "Process
        // meshes", "Process textures", "File setting", "Base", "Resizing", "More"), so fixing it
        // here fixes all of them at once.
        Text {
            anchors.verticalCenter: parent.verticalCenter
            color: NebulaTheme.accentBlue
            font.pixelSize: 13
            font.weight: Font.DemiBold
            text: root.title
        }
    }

    MouseArea {
        id: titleArea
        anchors.fill: titleRow
        hoverEnabled: true
        enabled: root.checkable
        onClicked: root.toggled(!root.checked)
    }

    Item {
        visible: root.contentVisible
        anchors.top: titleRow.bottom
        anchors.topMargin: NebulaTheme.spacingS
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Column {
            id: contentColumn
            x: NebulaTheme.spacingM
            y: NebulaTheme.spacingM
            width: parent.width - NebulaTheme.spacingXL
            spacing: NebulaTheme.spacingS
        }
    }
}
