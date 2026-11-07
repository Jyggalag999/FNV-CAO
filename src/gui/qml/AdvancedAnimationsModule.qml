// Full QML replacement for AdvancedAnimationsModule's internals - bound to an
// AdvancedAnimationsBridge instance exposed as the "bridge" context property. A single toggle:
// whether to compress .kf animations at all, plus whether to run the lossless dedupe pre-pass. Every other btu::kf::Settings field (ratio,
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

                CaoCheckBox {
                    checked: bridge.dedupe
                    text: "Remove redundant repeated keys (lossless)"
                    enabled: bridge.enabled
                    onToggled: (checked) => bridge.dedupe = checked
                }
            }

            // Error budget: every converted bone is verified against these at its keys and between
            // them; a bone that cannot meet them stays uncompressed. 0 disables a limit.
            CaoGroupBox {
                width: parent.width
                title: "Error limits (per bone)"

                Row {
                    spacing: NebulaTheme.spacingS
                    Text {
                        width: 190
                        anchors.verticalCenter: parent.verticalCenter
                        color: NebulaTheme.textPrimary
                        text: "Rotation (degrees)"
                    }
                    CaoTextField {
                        width: 90
                        text: bridge.maxRotDeg.toString()
                        enabled: bridge.enabled
                        onTextEdited: (t) => { const v = parseFloat(t); if (!isNaN(v) && v >= 0) bridge.maxRotDeg = v }
                    }
                }
                Row {
                    spacing: NebulaTheme.spacingS
                    Text {
                        width: 190
                        anchors.verticalCenter: parent.verticalCenter
                        color: NebulaTheme.textPrimary
                        text: "Translation (game units)"
                    }
                    CaoTextField {
                        width: 90
                        text: bridge.maxTrans.toString()
                        enabled: bridge.enabled
                        onTextEdited: (t) => { const v = parseFloat(t); if (!isNaN(v) && v >= 0) bridge.maxTrans = v }
                    }
                }
                Row {
                    spacing: NebulaTheme.spacingS
                    Text {
                        width: 190
                        anchors.verticalCenter: parent.verticalCenter
                        color: NebulaTheme.textPrimary
                        text: "Scale"
                    }
                    CaoTextField {
                        width: 90
                        text: bridge.maxScale.toString()
                        enabled: bridge.enabled
                        onTextEdited: (t) => { const v = parseFloat(t); if (!isNaN(v) && v >= 0) bridge.maxScale = v }
                    }
                }
            }
        }
    }
}
