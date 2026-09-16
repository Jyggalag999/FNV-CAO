// Step 5: QML replacement for MainWindow's old QPainter/eventFilter nebula background (see
// MainWindow::eventFilter and nebula_background_widget_ in MainWindow.cpp/.hpp). This is the
// direct equivalent of the old code's
// QPixmap::scaled(Qt::KeepAspectRatioByExpanding) + centered QPainter::drawPixmap():
// Image.PreserveAspectCrop scales to fill the item's bounds while preserving aspect ratio,
// cropping and centering as needed - same visual result, no manual painting required.
//
// Root item is a Rectangle (not a bare Image) with an explicit anchors.fill: parent on the Image
// inside it, matching every other QML file in this codebase (see e.g. GeneralBSAModule.qml) -
// a bare root Image relying solely on QQuickWidget::SizeRootObjectToView left it sized to the
// source picture's own natural pixel dimensions instead of the view, leaving the rest of the
// widget showing Qt's default white clear color (a highly visible bug against this app's dark
// theme). The Rectangle's dark fill is also a safety net against that same white showing through
// on any future load failure, instead of a jarring white flash.
//
// galaxy2.jpg (blue/magenta spiral) is MainWindow's background - the app's single largest, most-
// seen surface, so it carries the newer/more vivid of the two nebula photos. galaxy.jpg (the
// original, more purple/gold) now backs the secondary dialog windows instead - see
// NebulaDialogBackground.qml - so both images are actually on screen, not just one committed and
// one left in the repo unused.
//
// The dark scrim on top is the "wallpaper is a supporting element, not the main attraction" half
// of the design spec: this app's panels are already translucent over this image (see
// NebulaTheme.qml's panelAlpha and every Cao* control), which alone still let the raw photo
// dominate wherever a panel doesn't cover - the scrim tones the whole backdrop down uniformly
// first, so panels read as the clear visual foreground everywhere, not just where they happen to
// sit over a darker part of the photo.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: NebulaTheme.bgDeep

    Image {
        anchors.fill: parent
        source: "qrc:/nebula/galaxy2.jpg"
        fillMode: Image.PreserveAspectCrop
    }

    Rectangle {
        anchors.fill: parent
        color: NebulaTheme.bgDeep
        opacity: 0.45
    }
}
