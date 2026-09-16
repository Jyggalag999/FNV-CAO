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
    color: NebulaTheme.bgDeep

    NebulaDialogBackground {}

    Column {
        anchors.fill: parent
        anchors.margins: NebulaTheme.spacingM
        spacing: NebulaTheme.spacingS

        ListView {
            id: patternsView
            width: parent.width
            height: parent.height - 44
            clip: true
            spacing: NebulaTheme.spacingXS
            model: patternsModel

            delegate: Rectangle {
                width: patternsView.width
                height: 32
                radius: NebulaTheme.radiusS
                color: NebulaTheme.withAlpha(NebulaTheme.bgPanel, NebulaTheme.panelAlpha)
                border.width: 1
                border.color: NebulaTheme.borderColor

                Row {
                    anchors.fill: parent
                    anchors.margins: NebulaTheme.spacingXS
                    spacing: NebulaTheme.spacingS

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 1

                        Rectangle {
                            width: 14
                            height: 10
                            color: upArea.containsMouse ? NebulaTheme.buttonTopHover : NebulaTheme.buttonTopNormal
                            Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }
                            Text { anchors.centerIn: parent; color: NebulaTheme.textPrimary; font.pixelSize: 8; text: "▲" }
                            MouseArea { id: upArea; anchors.fill: parent; hoverEnabled: true; onClicked: patternsModel.requestMoveUp(index) }
                        }

                        Rectangle {
                            width: 14
                            height: 10
                            color: downArea.containsMouse ? NebulaTheme.buttonTopHover : NebulaTheme.buttonTopNormal
                            Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }
                            Text { anchors.centerIn: parent; color: NebulaTheme.textPrimary; font.pixelSize: 8; text: "▼" }
                            MouseArea { id: downArea; anchors.fill: parent; hoverEnabled: true; onClicked: patternsModel.requestMoveDown(index) }
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
                        color: NebulaTheme.textSecondary
                        text: model.isRegex ? "Regex" : "Glob"
                    }

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        radius: NebulaTheme.radiusS
                        // Secondary (pink) accent, not primary purple - the one destructive
                        // action in this row, worth a visually distinct color from everything
                        // else. Was a stray unrelated orange (#962d00) before.
                        color: removeArea.containsMouse ? NebulaTheme.accentSecondary : NebulaTheme.buttonTopNormal
                        Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }

                        Text {
                            anchors.centerIn: parent
                            color: NebulaTheme.textPrimary
                            text: "×"
                        }

                        MouseArea {
                            id: removeArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: patternsModel.requestRemove(index)
                        }
                    }
                }
            }
        }

        CaoButton {
            width: 100
            text: "New"
            onClicked: patternsModel.requestNew()
        }
    }
}
