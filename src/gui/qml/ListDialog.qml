// Step 4 module port. Full QML replacement for ListDialog's internals - bound to a
// ListDialogModel instance ("listModel") and the ListDialog itself ("dialog", for Close ->
// dialog.accept(), same public-slot-is-invokable trick used by SelectGpuDialog.qml).
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#0a0512"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text {
            color: "#e6d8ef"
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

        Rectangle {
            visible: listModel.addItemVisible
            width: 100
            height: 28
            color: addItemArea.pressed ? "#3c1450" : "#642878"

            Text {
                anchors.centerIn: parent
                color: "white"
                text: "Add item"
            }

            MouseArea {
                id: addItemArea
                anchors.fill: parent
                onClicked: listModel.requestAddItem()
            }
        }

        Rectangle {
            width: 100
            height: 28
            color: closeArea.pressed ? "#3c1450" : "#642878"

            Text {
                anchors.centerIn: parent
                color: "white"
                text: "Close"
            }

            MouseArea {
                id: closeArea
                anchors.fill: parent
                onClicked: dialog.accept()
            }
        }
    }
}
