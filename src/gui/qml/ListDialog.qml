// Step 4 module port. Full QML replacement for ListDialog's internals - bound to a
// ListDialogModel instance ("listModel") and the ListDialog itself ("dialog", for Close ->
// dialog.accept(), same public-slot-is-invokable trick used by SelectGpuDialog.qml).
import QtQuick

Rectangle {
    anchors.fill: parent
    color: NebulaTheme.bgDeep

    NebulaDialogBackground {}

    Column {
        anchors.fill: parent
        anchors.margins: NebulaTheme.spacingM
        spacing: NebulaTheme.spacingS

        Text {
            color: NebulaTheme.textPrimary
            text: "Search"
        }

        CaoTextField {
            width: parent.width
            text: listModel.filterText
            onTextEdited: (text) => listModel.filterText = text
        }

        ListView {
            id: listView
            width: parent.width
            height: 260
            clip: true
            spacing: 2
            model: listModel

            delegate: CaoCheckBox {
                width: listView.width
                height: visible ? implicitHeight : 0
                visible: !model.hidden
                checked: model.checked
                text: model.text
                onToggled: listModel.toggleChecked(index)
            }
        }

        CaoButton {
            visible: listModel.addItemVisible
            width: 100
            text: "Add item"
            onClicked: listModel.requestAddItem()
        }

        CaoButton {
            width: 100
            text: "Close"
            onClicked: dialog.accept()
        }
    }
}
