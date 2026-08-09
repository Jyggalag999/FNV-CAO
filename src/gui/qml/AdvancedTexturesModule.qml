// Step 4 module port. Full QML replacement for AdvancedTexturesModule's internals - bound to an
// AdvancedTexturesBridge instance exposed as the "bridge" context property.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#170c26"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 16

        CaoGroupBox {
            width: parent.width
            title: "Process textures"
            checkable: true
            checked: bridge.mainChecked
            onToggled: (checked) => bridge.mainChecked = checked

            Row {
                visible: bridge.mainChecked
                spacing: 16

                CaoCheckBox {
                    checked: bridge.compress
                    text: "Compress textures"
                    onToggled: (checked) => bridge.compress = checked
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
        }

        CaoGroupBox {
            width: parent.width
            title: "Resizing"
            checkable: true
            checked: bridge.resizingChecked
            onToggled: (checked) => bridge.resizingChecked = checked

            Column {
                visible: bridge.resizingChecked
                spacing: 8

                Row {
                    spacing: 24

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
                    spacing: 16

                    Column {
                        spacing: 2
                        Text { color: "#e6d8ef"; text: "Width" }
                        CaoSpinBox {
                            from: 2
                            to: 8192
                            value: bridge.width
                            onValueEdited: (v) => bridge.width = bridge.roundUpPow2(v)
                        }
                    }

                    Column {
                        spacing: 2
                        Text { color: "#e6d8ef"; text: "Height" }
                        CaoSpinBox {
                            from: 2
                            to: 8192
                            value: bridge.height
                            onValueEdited: (v) => bridge.height = bridge.roundUpPow2(v)
                        }
                    }
                }

                Row {
                    spacing: 8
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
