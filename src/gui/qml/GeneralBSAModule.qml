// Step 4 module port. Full QML replacement for GeneralBSAModule's internals - bound to a
// GeneralBSABridge instance exposed as the "bridge" context property.
//
// Root is a plain (transparent) Item filling the tab page, not the visible purple box itself -
// see the Rectangle below, sized to its own content (matching CaoGroupBox.qml's technique)
// rather than stretching to fill the tab page's full height. The old anchors.fill: parent
// Rectangle left a large empty stretch of solid color below the last row whenever the tab page
// was taller than the content (which it usually is, since QTabWidget sizes every page to match
// the widest/tallest one). Leaving the root Item transparent lets QTabWidget::pane's own
// background (#0d0818, from MainWindow.cpp's nebula_overrides) show through that leftover space
// instead - a clean, intentional-looking transition rather than dead purple space.
import QtQuick

Item {
    id: root
    anchors.fill: parent
    // Reports the real content height back up through QQuickWidget::sizeHint() (rootObject()'s
    // implicitSize, regardless of resize mode) - see GeneralBSAModule.cpp - so MainWindow can
    // size its initial window height to fit instead of leaving dead space below.
    implicitHeight: contentBox.height

    // QML-local toggle for the "More" box's own visibility. No corresponding Settings field exists
    // for this - it's a new UI-only control (not present in the original Widgets app) added per
    // request to let "More" collapse independently of "Process BSAs".
    property bool moreChecked: true

    // Unboxed (plain Item, no fill/border) - was a translucent Rectangle, removed per feedback
    // that the stacked boxes (this one, CaoGroupBox's own nested box inside it, QTabWidget::pane
    // further out) made the UI read as "boxes on boxes" over the nebula rather than the nebula
    // itself being the focus. MainWindow's single shared nebula_background_widget_ (see
    // NebulaBackground.qml) already shows straight through a plain Item with nothing to block it.
    Item {
        id: contentBox
        width: parent.width
        height: column.implicitHeight + NebulaTheme.spacingXL

        Column {
            id: column
            x: NebulaTheme.spacingM
            y: NebulaTheme.spacingM
            width: parent.width - NebulaTheme.spacingXL
            spacing: NebulaTheme.spacingL

            CaoGroupBox {
                width: parent.width
                title: "Process BSAs"
                checkable: true
                checked: bridge.baseChecked
                onToggled: (checked) => bridge.baseChecked = checked

                Row {
                    spacing: NebulaTheme.spacingXL

                    CaoRadioButton {
                        checked: bridge.extractMode
                        text: "Extract BSAs"
                        onClicked: bridge.extractMode = true
                    }

                    CaoRadioButton {
                        checked: !bridge.extractMode
                        text: "Create BSAs"
                        onClicked: bridge.extractMode = false
                    }
                }
            }

            CaoGroupBox {
                width: parent.width
                title: "More"
                checkable: true
                checked: root.moreChecked
                onToggled: (checked) => root.moreChecked = checked

                CaoCheckBox {
                    checked: bridge.dontMakeLoaded
                    text: "Do not make BSA loaded"
                    onToggled: (checked) => bridge.dontMakeLoaded = checked
                }

                CaoCheckBox {
                    checked: bridge.dontCompress
                    text: "Do not compress files"
                    onToggled: (checked) => bridge.dontCompress = checked
                }

                CaoCheckBox {
                    checked: bridge.dontRemoveFiles
                    text: "Do not remove packed files"
                    onToggled: (checked) => bridge.dontRemoveFiles = checked
                }

                CaoCheckBox {
                    visible: bridge.makeOverridesVisible
                    checked: bridge.makeOverrides
                    text: "Create override files"
                    onToggled: (checked) => bridge.makeOverrides = checked
                }

                Row {
                    spacing: NebulaTheme.spacingS

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: NebulaTheme.textPrimary
                        text: "Archive name:"
                    }

                    CaoTextField {
                        anchors.verticalCenter: parent.verticalCenter
                        text: bridge.archiveName
                        placeholderText: "Leave blank for automatic naming"
                        onTextEdited: (text) => bridge.archiveName = text
                    }
                }

                Row {
                    spacing: NebulaTheme.spacingS

                    CaoCheckBox {
                        anchors.verticalCenter: parent.verticalCenter
                        checked: bridge.overrideMaxSize
                        text: "Max size to pack BSAs to (MB):"
                        onToggled: (checked) => bridge.overrideMaxSize = checked
                    }

                    CaoSpinBox {
                        anchors.verticalCenter: parent.verticalCenter
                        enabled: bridge.overrideMaxSize
                        from: 1
                        to: 8192
                        value: bridge.maxSizeMb
                        onValueEdited: (v) => bridge.maxSizeMb = v
                    }
                }

                Text {
                    color: NebulaTheme.textSecondary
                    font.italic: true
                    wrapMode: Text.WordWrap
                    width: parent.width
                    text: "Archives are split into multiple parts once they'd exceed this size " +
                          "(game default: " + bridge.defaultMaxSizeMb + " MB). Most archive " +
                          "formats get unreliable right around 4096 MB, so stay comfortably under " +
                          "that."
                }
            }
        }
    }
}
