// Step 4 module port. Full QML replacement for AdvancedMeshesModule's internals - bound to an
// AdvancedMeshesBridge instance exposed as the "bridge" context property.
//
// Root is a plain (transparent) Item filling the tab page, not the visible purple box itself -
// see the Rectangle below, sized to its own content (matching CaoGroupBox.qml's technique)
// rather than stretching to fill the tab page's full height, which left a large empty stretch of
// solid color below the content. See GeneralBSAModule.qml for the full rationale.
import QtQuick

Item {
    anchors.fill: parent
    // Reports the real content height back up through QQuickWidget::sizeHint() (rootObject()'s
    // implicitSize, regardless of resize mode) - see AdvancedMeshesModule.cpp - so MainWindow can
    // size its initial window height to fit instead of leaving dead space below.
    implicitHeight: contentBox.height

    Rectangle {
        id: contentBox
        width: parent.width
        height: column.implicitHeight + 24
        color: "#170c26"

        Column {
            id: column
            x: 12
            y: 12
            width: parent.width - 24
            spacing: 8

            CaoGroupBox {
                width: parent.width
                title: "Process meshes"
                checkable: true
                checked: bridge.baseChecked
                onToggled: (checked) => bridge.baseChecked = checked

                Row {
                    visible: bridge.baseChecked
                    spacing: 24

                    CaoRadioButton {
                        checked: !bridge.fullOptimization
                        text: "Recommended"
                        onClicked: bridge.fullOptimization = false
                    }

                    CaoRadioButton {
                        checked: bridge.fullOptimization
                        text: "Extensive"
                        onClicked: bridge.fullOptimization = true
                    }
                }
            }
        }
    }
}
