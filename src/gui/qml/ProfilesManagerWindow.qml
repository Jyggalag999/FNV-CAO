// Step 4 module port. Full QML replacement for ProfilesManagerWindow's internals - bound to a
// ProfilesManagerBridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#0a0512"

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Row {
            spacing: 8

            Text {
                width: 130
                anchors.verticalCenter: parent.verticalCenter
                color: "#e6d8ef"
                text: "Profile"
            }

            CaoComboBox {
                anchors.verticalCenter: parent.verticalCenter
                options: bridge.profileNames
                currentIndex: bridge.profileNames.indexOf(bridge.currentProfile)
                onActivated: (index) => bridge.requestSelectProfile(bridge.profileNames[index])
            }
        }

        Row {
            spacing: 8

            Text {
                width: 130
                anchors.verticalCenter: parent.verticalCenter
                color: "#e6d8ef"
                text: "Corresponding game"
            }

            CaoComboBox {
                anchors.verticalCenter: parent.verticalCenter
                options: bridge.gameNames
                currentIndex: bridge.currentGameIndex
                onActivated: (index) => bridge.requestSelectGame(index)
            }
        }

        Row {
            spacing: 8

            Rectangle {
                width: 130
                height: 28
                color: newArea.pressed ? "#280028" : "#3c1450"
                Text { anchors.centerIn: parent; color: "white"; text: "New" }
                MouseArea { id: newArea; anchors.fill: parent; onClicked: bridge.requestNewProfile() }
            }

            Rectangle {
                width: 130
                height: 28
                color: removeArea.pressed ? "#280028" : "#3c1450"
                Text { anchors.centerIn: parent; color: "white"; text: "Remove" }
                MouseArea { id: removeArea; anchors.fill: parent; onClicked: bridge.requestRemoveProfile() }
            }
        }

        Row {
            spacing: 8

            Rectangle {
                width: 130
                height: 28
                color: importArea.pressed ? "#280028" : "#3c1450"
                Text { anchors.centerIn: parent; color: "white"; text: "Import" }
                MouseArea { id: importArea; anchors.fill: parent; onClicked: bridge.requestImportProfile() }
            }

            Rectangle {
                width: 130
                height: 28
                color: exportArea.pressed ? "#280028" : "#3c1450"
                Text { anchors.centerIn: parent; color: "white"; text: "Export" }
                MouseArea { id: exportArea; anchors.fill: parent; onClicked: bridge.requestExportProfile() }
            }
        }
    }
}
