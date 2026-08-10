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
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#0a0512"

    Image {
        anchors.fill: parent
        source: "qrc:/nebula/galaxy.jpg"
        fillMode: Image.PreserveAspectCrop
    }
}
