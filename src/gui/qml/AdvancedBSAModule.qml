// Step 4 module port. Full QML replacement for AdvancedBSAModule's internals - bound to an
// AdvancedBSABridge instance exposed as the "bridge" context property.
//
// Root is a plain (transparent) Item filling the tab page, not the visible purple box itself -
// see the Rectangle below, sized to its own content (matching CaoGroupBox.qml's technique)
// rather than stretching to fill the tab page's full height, which left a large empty stretch of
// solid color below the content. See GeneralBSAModule.qml for the full rationale.
import QtQuick

Item {
    anchors.fill: parent
    // Reports the real content height back up through QQuickWidget::sizeHint() (rootObject()'s
    // implicitSize, regardless of resize mode) - see AdvancedBSAModule.cpp - so MainWindow can
    // size its initial window height to fit instead of leaving dead space below.
    implicitHeight: contentBox.height

    // Unboxed - see GeneralBSAModule.qml for the full rationale (same module family).
    Item {
        id: contentBox
        width: parent.width
        height: column.implicitHeight + NebulaTheme.spacingXL

        Column {
            id: column
            x: NebulaTheme.spacingM
            y: NebulaTheme.spacingM
            width: parent.width - NebulaTheme.spacingXL
            spacing: NebulaTheme.spacingS

            CaoGroupBox {
                width: parent.width
                title: "File setting"

                CaoCheckBox {
                    checked: bridge.packFile
                    text: "Pack file"
                    onToggled: (checked) => bridge.packFile = checked
                }
            }
        }
    }
}
