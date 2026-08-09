// Step 1 build/link/embed proof-of-concept for the QQuickWidget migration.
// Deliberately trivial - no real UI content is ported here. See MainWindow.cpp for how
// this is embedded, and the migration plan for what comes next.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#ff6600"

    Text {
        anchors.centerIn: parent
        text: "QQuickWidget POC"
        color: "white"
        font.pixelSize: 18
    }
}
