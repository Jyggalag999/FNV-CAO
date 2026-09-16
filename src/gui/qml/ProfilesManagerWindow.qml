// Step 4 module port. Full QML replacement for ProfilesManagerWindow's internals - bound to a
// ProfilesManagerBridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: NebulaTheme.bgDeep

    NebulaDialogBackground {}

    Column {
        anchors.fill: parent
        anchors.margins: NebulaTheme.spacingL
        spacing: NebulaTheme.spacingM

        Row {
            spacing: NebulaTheme.spacingS

            Text {
                width: 130
                anchors.verticalCenter: parent.verticalCenter
                color: NebulaTheme.textPrimary
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
            spacing: NebulaTheme.spacingS

            Text {
                width: 130
                anchors.verticalCenter: parent.verticalCenter
                color: NebulaTheme.textPrimary
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
            spacing: NebulaTheme.spacingS

            CaoButton { width: 130; text: "New"; onClicked: bridge.requestNewProfile() }
            CaoButton { width: 130; text: "Remove"; onClicked: bridge.requestRemoveProfile() }
        }

        Row {
            spacing: NebulaTheme.spacingS

            CaoButton { width: 130; text: "Import"; onClicked: bridge.requestImportProfile() }
            CaoButton { width: 130; text: "Export"; onClicked: bridge.requestExportProfile() }
        }
    }
}
