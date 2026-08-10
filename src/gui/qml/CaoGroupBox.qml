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

    implicitWidth: contentColumn.implicitWidth + 24
    implicitHeight: titleRow.implicitHeight + (root.contentVisible ? 6 + contentColumn.implicitHeight + 24 : 0)

    Row {
        id: titleRow
        spacing: 6

        Rectangle {
            visible: root.checkable
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
            color: "#d98fe0"
            font.pixelSize: 13
            text: root.title
        }
    }

    MouseArea {
        anchors.fill: titleRow
        enabled: root.checkable
        onClicked: root.toggled(!root.checked)
    }

    Rectangle {
        visible: root.contentVisible
        anchors.top: titleRow.bottom
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        radius: 3
        color: "#120a1e"
        border.color: "#4a2c6d"

        Column {
            id: contentColumn
            x: 12
            y: 12
            width: parent.width - 24
            spacing: 8
        }
    }
}
