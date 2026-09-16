// Full QML replacement for AdvancedAnimationsModule's internals - bound to an
// AdvancedAnimationsBridge instance exposed as the "bridge" context property. A single toggle:
// whether to compress .kf animations at all. Every other btu::kf::Settings field (ratio,
// compact16, blacklist, etc.) is hardcoded in AdvancedAnimationsModule.cpp to the known-good/
// vanilla-matching values, not exposed here - see that file for why.
//
// Root is a plain (transparent) Item filling the tab page, not the visible purple box itself -
// see the Rectangle below, sized to its own content (matching CaoGroupBox.qml's technique)
// rather than stretching to fill the tab page's full height. See GeneralBSAModule.qml for the
// full rationale.
import QtQuick

Item {
    anchors.fill: parent
    // Reports the real content height back up through QQuickWidget::sizeHint() (rootObject()'s
    // implicitSize, regardless of resize mode) - see AdvancedAnimationsModule.cpp - so MainWindow
    // can size its initial window height to fit instead of leaving dead space below.
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
                title: "Animations"

                CaoCheckBox {
                    checked: bridge.enabled
                    text: "Compress animations"
                    onToggled: (checked) => bridge.enabled = checked
                }
            }
        }
    }
}
