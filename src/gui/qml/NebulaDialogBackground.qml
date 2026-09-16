// Shared background for the app's secondary dialog windows (Profiles Manager, Patterns Manager,
// Progress, Select GPU, List Dialog, Level Selector) - previously flat "#0a0512" with nothing
// else, unlike MainWindow which has always had a real nebula photo behind it (see
// NebulaBackground.qml). Gives them the same nebula treatment using the *original* galaxy.jpg
// (MainWindow itself uses the newer galaxy2.jpg) - so both nebula photos are actually visible
// somewhere in the app, not one committed to the repo and never shown.
//
// Not a straight copy of NebulaBackground.qml: these dialogs pack much smaller, denser text/
// controls than MainWindow's few large panels, so the raw photo needs to recede further before
// everything stays comfortably legible over it - the scrim Rectangle stacked on top (bgDeep,
// partially transparent) does that without needing a second image asset or a blur/effects module
// this build doesn't otherwise depend on.
//
// Instantiate as the first child of a dialog's root Rectangle (before its actual content), not as
// a drop-in replacement for that root - every consumer already has its own root Rectangle with
// anchors.fill/color serving as the pre-image-load fallback + Qt Quick's z-ordering (each
// dialog's real Column content is declared after this, so it paints on top).
import QtQuick

Item {
    anchors.fill: parent

    Image {
        anchors.fill: parent
        source: "qrc:/nebula/galaxy.jpg"
        fillMode: Image.PreserveAspectCrop
    }

    Rectangle {
        anchors.fill: parent
        color: NebulaTheme.bgDeep
        // Was 0.65 - bolder now (photo clearly visible, matching NebulaTheme.panelAlpha's
        // updated direction), but not dropped as far as that token: these dialogs have plain
        // Text labels sitting directly on this scrim with no further panel/tint layered on top
        // the way MainWindow's controls do, so this keeps a slightly higher legibility floor.
        opacity: 0.5
    }
}
