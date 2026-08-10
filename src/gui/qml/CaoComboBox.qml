// Shared hand-rolled dropdown, matching the app's existing dark palette. Built for
// ProfilesManagerWindow's profile list, which (unlike AdvancedTexturesModule's 2-item resize mode
// or GeneralBSAModule's 2-item BSA operation) is open-ended, so a CaoRadioButton row doesn't scale
// - this is the first module needing a real dropdown.
import QtQuick

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

    Rectangle {
        id: popup
        visible: false
        z: 100
        anchors.top: box.bottom
        anchors.left: box.left
        width: box.width
        height: Math.min(listColumn.implicitHeight, 200)
        color: "#170c26"
        border.color: "#4a2c6d"
        clip: true

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
