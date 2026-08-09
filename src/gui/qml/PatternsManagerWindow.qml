// Step 4 module port. Full QML replacement for PatternsManagerWindow's internals - bound to a
// PatternsManagerModel instance exposed as the "patternsModel" context property.
//
// Two deliberate UX substitutions from the old Widgets version, both noted in the port's commit:
// drag-and-drop reordering is replaced with per-row up/down buttons, and the single external
// "Remove" button (enabled only when a row is selected) is replaced with a per-row remove button
// - simpler than reintroducing row-selection state for what a per-row action handles directly.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#0a0512"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        ListView {
            id: patternsView
            width: parent.width
            height: parent.height - 44
            clip: true
            spacing: 4
            model: patternsModel

            delegate: Rectangle {
                width: patternsView.width
                height: 32
                color: "#170c26"
                border.color: "#4a2c6d"

                Row {
                    anchors.fill: parent
                    anchors.margins: 4
                    spacing: 6

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 1

                        Rectangle {
                            width: 14
                            height: 10
                            color: upArea.pressed ? "#280028" : "#3c1450"
                            Text { anchors.centerIn: parent; color: "white"; font.pixelSize: 8; text: "▲" }
                            MouseArea { id: upArea; anchors.fill: parent; onClicked: patternsModel.requestMoveUp(index) }
                        }

                        Rectangle {
                            width: 14
                            height: 10
                            color: downArea.pressed ? "#280028" : "#3c1450"
                            Text { anchors.centerIn: parent; color: "white"; font.pixelSize: 8; text: "▼" }
                            MouseArea { id: downArea; anchors.fill: parent; onClicked: patternsModel.requestMoveDown(index) }
                        }
                    }

                    CaoTextField {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 180
                        text: model.text
                        onTextEdited: (text) => patternsModel.requestRename(index, text)
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#c4a8d4"
                        text: model.isRegex ? "Regex" : "Glob"
                    }

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        color: removeArea.pressed ? "#962d00" : "#3c1450"

                        Text {
                            anchors.centerIn: parent
                            color: "white"
                            text: "×"
                        }

                        MouseArea {
                            id: removeArea
                            anchors.fill: parent
                            onClicked: patternsModel.requestRemove(index)
                        }
                    }
                }
            }
        }

        Rectangle {
            width: 100
            height: 28
            color: newArea.pressed ? "#280028" : "#3c1450"

            Text {
                anchors.centerIn: parent
                color: "white"
                text: "New"
            }

            MouseArea {
                id: newArea
                anchors.fill: parent
                onClicked: patternsModel.requestNew()
            }
        }
    }
}
