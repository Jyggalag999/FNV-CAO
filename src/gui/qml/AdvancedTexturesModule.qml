// Step 4 module port. Full QML replacement for AdvancedTexturesModule's internals - bound to an
// AdvancedTexturesBridge instance exposed as the "bridge" context property.
//
// Root is a plain (transparent) Item filling the tab page, not the visible purple box itself -
// see the Rectangle below, sized to its own content (matching CaoGroupBox.qml's technique)
// rather than stretching to fill the tab page's full height, which left a large empty stretch of
// solid color below the content. See GeneralBSAModule.qml for the full rationale.
import QtQuick
import QtQuick.Controls

Item {
    anchors.fill: parent
    // Reports the real content height back up through QQuickWidget::sizeHint() (rootObject()'s
    // implicitSize, regardless of resize mode) - see AdvancedTexturesModule.cpp - so MainWindow
    // can size its initial window height to fit instead of leaving dead space below.
    implicitHeight: contentBox.height

    // Unboxed - see GeneralBSAModule.qml for the full rationale (same module family).
    // Scrolls when the content is taller than the tab (instead of being clipped).
    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: contentBox.height
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: ScrollBar { policy: size < 1.0 ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff } // visible whenever content overflows (AsNeeded hides it until scrolled)

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
                title: "Process textures"
                checkable: true
                checked: bridge.mainChecked
                onToggled: (checked) => bridge.mainChecked = checked

                Column {
                    width: parent.width
                    visible: bridge.mainChecked
                    spacing: NebulaTheme.spacingS

                    Flow {
                        width: parent.width
                        spacing: NebulaTheme.spacingL

                        CaoCheckBox {
                            checked: bridge.compress
                            text: "Compress textures"
                            onToggled: (checked) => bridge.compress = checked
                        }

                        CaoCheckBox {
                            checked: bridge.compressUncompressedOnly
                            text: "Compress uncompressed only"
                            onToggled: (checked) => bridge.compressUncompressedOnly = checked
                        }

                        CaoCheckBox {
                            checked: bridge.forceCrunch
                            text: "Use crunch encoder"
                            onToggled: (checked) => bridge.forceCrunch = checked
                        }

                        CaoCheckBox {
                            checked: bridge.mipmaps
                            text: "Generate mipmaps"
                            onToggled: (checked) => bridge.mipmaps = checked
                        }
                    }

                    CaoCheckBox {
                        checked: bridge.bc3ToBc1a
                        text: "Store binary-alpha diffuse DXT5 as DXT1 (1-bit alpha)"
                        onToggled: (checked) => bridge.bc3ToBc1a = checked
                    }

                    Text {
                        color: NebulaTheme.textPrimary
                        opacity: 0.7
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: "Only diffuse textures whose alpha is exactly 0 or 255 everywhere. Transparent pixels turn black (colour data under alpha 0 is dropped); normal/glow/specular maps are never touched."
                    }
                }
            }

            CaoGroupBox {
                width: parent.width
                title: "Resizing"
                checkable: true
                checked: bridge.resizingChecked
                onToggled: (checked) => bridge.resizingChecked = checked

                Column {
                    width: parent.width
                    visible: bridge.resizingChecked
                    spacing: NebulaTheme.spacingS

                    Row {
                        spacing: NebulaTheme.spacingXL

                        CaoRadioButton {
                            checked: bridge.resizeByRatio
                            text: "By ratio"
                            onClicked: bridge.resizeByRatio = true
                        }

                        CaoRadioButton {
                            checked: !bridge.resizeByRatio
                            text: "By fixed size"
                            onClicked: bridge.resizeByRatio = false
                        }
                    }

                    Row {
                        spacing: NebulaTheme.spacingL

                        Column {
                            spacing: 2
                            Text { color: NebulaTheme.textPrimary; text: "Width" }
                            CaoSpinBox {
                                from: 2
                                to: 8192
                                value: bridge.width
                                onValueEdited: (v) => bridge.width = bridge.roundUpPow2(v)
                            }
                        }

                        Column {
                            spacing: 2
                            Text { color: NebulaTheme.textPrimary; text: "Height" }
                            CaoSpinBox {
                                from: 2
                                to: 8192
                                value: bridge.height
                                onValueEdited: (v) => bridge.height = bridge.roundUpPow2(v)
                            }
                        }
                    }

                    Row {
                        spacing: NebulaTheme.spacingS
                        visible: bridge.resizeByRatio

                        CaoCheckBox {
                            anchors.verticalCenter: parent.verticalCenter
                            checked: bridge.minimumChecked
                            text: "Minimum size:"
                            onToggled: (checked) => bridge.minimumChecked = checked
                        }

                        CaoSpinBox {
                            anchors.verticalCenter: parent.verticalCenter
                            enabled: bridge.minimumChecked
                            from: 4
                            to: 8192
                            value: bridge.minimumWidth
                            onValueEdited: (v) => bridge.minimumWidth = bridge.roundUpPow2(v)
                        }

                        CaoSpinBox {
                            anchors.verticalCenter: parent.verticalCenter
                            enabled: bridge.minimumChecked
                            from: 4
                            to: 8192
                            value: bridge.minimumHeight
                            onValueEdited: (v) => bridge.minimumHeight = bridge.roundUpPow2(v)
                        }
                    }
                }
            }
        }
    }
    }
}
