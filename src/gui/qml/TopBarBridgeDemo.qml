// Step 2 verification scaffold for TopBarBridge (src/gui/TopBarBridge.hpp). Supersedes Step 1's
// PocRectangle.qml, which already proved the QQuickWidget build/link/embed mechanics - this
// widget's job now is proving Q_PROPERTY/Q_INVOKABLE/signal round-trips against real Settings
// data. Not real UI - the actual top bar gets ported later, per the migration plan.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#20304a"

    Column {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        Text { color: "white"; font.pixelSize: 12; text: "Profile: " + topBar.currentProfile }
        Text { color: "white"; font.pixelSize: 12; text: "Profiles: " + topBar.profileList.join(", ") }
        Text { color: "white"; font.pixelSize: 12; text: "Pattern: " + topBar.currentPattern }

        Rectangle {
            width: 80
            height: 24
            color: runArea.pressed ? "#3c1450" : "#642878"

            Text {
                anchors.centerIn: parent
                color: "white"
                text: "Run"
            }

            MouseArea {
                id: runArea
                anchors.fill: parent
                onClicked: topBar.run()
            }
        }
    }
}
