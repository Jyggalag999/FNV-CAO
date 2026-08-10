// Shared hand-rolled dropdown, matching the app's existing dark palette. Built for
// ProfilesManagerWindow's profile list, which (unlike AdvancedTexturesModule's 2-item resize mode
// or GeneralBSAModule's 2-item BSA operation) is open-ended, so a CaoRadioButton row doesn't scale
// - this is the first module needing a real dropdown.
import QtQuick
import QtQuick.Window

Item {
    id: root

    property var options: []
    property int currentIndex: -1
    property string placeholderText: "Select..."

    // Exposed so a host embedding this in a tightly-sized QQuickWidget (one not already big
    // enough to have slack room below the box) can grow itself while the popup is open - see
    // TopBar.qml/MainWindow.cpp, where the popup was otherwise getting clipped by the view's own
    // bounds. Not needed when there's already plenty of room (e.g. ProfilesManagerWindow.qml).
    readonly property alias popupOpen: popup.visible

    signal activated(int index)

    implicitWidth: 200
    implicitHeight: 24

    Rectangle {
        id: box
        anchors.fill: parent
        radius: 3
        color: "#170c26"
        border.color: boxArea.containsMouse ? "#d98fe0" : "#4a2c6d"

        Text {
            anchors.left: parent.left
            anchors.right: arrow.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 6
            color: "#e6d8ef"
            elide: Text.ElideRight
            text: root.currentIndex >= 0 && root.currentIndex < root.options.length
                  ? root.options[root.currentIndex] : root.placeholderText
        }

        Text {
            id: arrow
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 6
            color: "#c4a8d4"
            text: popup.visible ? "▲" : "▼"
        }

        MouseArea {
            id: boxArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: popup.visible = !popup.visible
        }
    }

    // Reparented to the window's content item (rather than left as a plain child of root, sized/
    // positioned via anchors.top: box.bottom/anchors.left: box.left) whenever it opens - see
    // reposition() below. z: 100 only ever raised this popup above box, its *own* sibling within
    // this component - it did nothing for this whole CaoComboBox's siblings in whatever it's
    // embedded in (e.g. TopBar.qml's "Pattern" row, declared right after "Profile"'s), which then
    // painted on top of an open Profile popup by ordinary document-order stacking, since raising
    // z locally inside one component doesn't elevate the component itself in its parent's
    // stacking order. Reparenting to the window's own root item escapes that local stacking
    // context entirely, so this can never lose to a sibling again regardless of where this
    // component is used. anchors can't reach across that reparenting (they only resolve between
    // items sharing a direct parent/child relationship) - explicit x/y computed via
    // box.mapToItem() at open time replace them.
    Rectangle {
        id: popup
        parent: root.Window.contentItem ?? root
        visible: false
        z: 1000
        width: box.width
        height: Math.min(listColumn.implicitHeight, 200)
        color: "#170c26"
        border.color: "#4a2c6d"
        clip: true

        function reposition() {
            const pos = box.mapToItem(popup.parent, 0, box.height);
            popup.x = pos.x;
            popup.y = pos.y;
        }

        onVisibleChanged: if (visible) reposition()

        Flickable {
            anchors.fill: parent
            contentHeight: listColumn.implicitHeight
            clip: true

            Column {
                id: listColumn
                width: popup.width

                Repeater {
                    model: root.options

                    delegate: Rectangle {
                        width: popup.width
                        height: 24
                        color: optionArea.containsMouse ? "#3c1450" : "transparent"

                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin: 6
                            color: "#e6d8ef"
                            text: modelData
                        }

                        MouseArea {
                            id: optionArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                popup.visible = false;
                                root.activated(index);
                            }
                        }
                    }
                }
            }
        }
    }
}
