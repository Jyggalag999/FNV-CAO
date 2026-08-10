// Step 2 verification scaffold for TopBarBridge (src/gui/TopBarBridge.hpp). Supersedes Step 1's
// PocRectangle.qml, which already proved the QQuickWidget build/link/embed mechanics - this
// widget's job now is proving Q_PROPERTY/Q_INVOKABLE/signal round-trips against real Settings
// data. Not real UI - the actual top bar gets ported later, per the migration plan.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#170c26"
    // Border/radius matches the profiles/patterns QComboBox bars beside it (see the
    // QSpinBox/QComboBox rule in MainWindow.cpp's nebula_overrides) so this panel reads as part
    // of the same styled group instead of a flat, borderless leftover.
    border.color: "#4a2c6d"
    border.width: 1
    radius: 3

    Column {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        // Lavender body text, matching QLabel's overridden color - plain "white" read as a
        // native/default-widget leftover next to the rest of the app's palette.
        Text { color: "#e6d8ef"; font.pixelSize: 12; text: "Profile: " + topBar.currentProfile }
        Text { color: "#e6d8ef"; font.pixelSize: 12; text: "Profiles: " + topBar.profileList.join(", ") }
        Text { color: "#e6d8ef"; font.pixelSize: 12; text: "Pattern: " + topBar.currentPattern }

        Rectangle {
            id: runButton
            width: 80
            height: 24
            radius: 4
            border.color: runArea.containsMouse ? "#642878" : "#4a2c6d"
            border.width: 1
            color: runArea.pressed ? "#962d00" : (runArea.containsMouse ? "#50145a" : "#3c1450")

            Text {
                anchors.centerIn: parent
                color: "#f0e0f5"
                text: "Run"
            }

            MouseArea {
                id: runArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: topBar.run()
            }
        }
    }
}
