// Shared ComboBox, matching the app's existing dark palette. Built on QtQuick.Controls.Basic's
// real ComboBox control - not a hand-rolled Item mimicking one with its own MouseArea and a
// sibling Popup - so opening/closing, keyboard handling, and hover state all come from Qt's own
// implementation instead of being reimplemented here. background/contentItem/indicator/popup are
// all fully overridden to match the palette; Basic is the unstyled base style, so no native/
// platform chrome bleeds in around them (the same problem this app already hit once with native
// QComboBox popups ignoring its palette on Windows - see MainWindow.cpp's nebula_overrides
// history).
//
// popup's parent is Overlay.overlay, not this control or any layout: Overlay.overlay is the
// generic per-window overlay layer QtQuick.Controls exposes as an attached property on *any*
// window (Popup/Drawer/etc. all resolve it lazily against whichever QQuickWindow they're shown
// in) - this app has no ApplicationWindow anywhere (QML is loaded as fragments into QQuickWidget,
// not as a full ApplicationWindow app), so ApplicationWindow.overlay specifically would resolve to
// nothing; Overlay.overlay is what that property is a convenience alias for, and it works the same
// with or without one. Parenting there fully detaches the dropdown from this control's (and its
// host's) layout tree, so it floats as a real, independent layer instead of ever being able to
// push sibling content down or get bounded by a small host's own layout.
import QtQuick
import QtQuick.Controls.Basic

ComboBox {
    id: root

    // Preserves the existing "options"/call-site API (TopBar.qml, ProfilesManagerWindow.qml) -
    // ComboBox's own "model" is the real backing property this aliases, so no call site needs to
    // change. currentIndex and the activated(int index) signal used at every call site are
    // ComboBox's own native members already - not redeclared here.
    property alias options: root.model
    property string placeholderText: "Select..."

    // Exposed so a host embedding this in a tightly-sized QQuickWidget (one not already big
    // enough to have slack room below the box) can grow itself while the popup is open - see
    // TopBar.qml/MainWindow.cpp, where the popup was otherwise getting clipped by the view's own
    // bounds. Not needed when there's already plenty of room (e.g. ProfilesManagerWindow.qml).
    readonly property alias popupOpen: root.popup.visible

    implicitWidth: 200
    implicitHeight: NebulaTheme.controlHeight

    background: Rectangle {
        radius: NebulaTheme.radiusS
        color: NebulaTheme.bgInput
        border.width: root.activeFocus ? 2 : 1
        border.color: root.activeFocus
                      ? NebulaTheme.borderColorFocus
                      : (root.hovered ? NebulaTheme.borderColorStrong : NebulaTheme.borderColor)
        Behavior on border.color { ColorAnimation { duration: NebulaTheme.durationNormal } }
    }

    contentItem: Text {
        leftPadding: NebulaTheme.spacingS
        rightPadding: root.indicator.width + NebulaTheme.spacingS
        verticalAlignment: Text.AlignVCenter
        color: NebulaTheme.textPrimary
        elide: Text.ElideRight
        text: root.currentIndex >= 0 && root.currentIndex < root.count
              ? root.textAt(root.currentIndex) : root.placeholderText
    }

    // Cyan, matching every other dropdown affordance in the app - see design spec's Dropdowns
    // section ("Dropdown arrows should use the cyan highlight color").
    indicator: Text {
        x: root.width - width - NebulaTheme.spacingS
        y: (root.height - height) / 2
        color: NebulaTheme.accentHighlight
        text: root.popup.visible ? "▲" : "▼"
    }

    popup: Popup {
        id: popup
        parent: Overlay.overlay
        modal: false
        width: root.width
        // Deterministic content height - root.count (ComboBox's own, always-accurate item count)
        // times the delegate's fixed height, capped at 200px with scrolling beyond that.
        // Previously bound to listView.contentHeight (a ListView/Flickable-computed property)
        // instead, which didn't reliably reflect the true delegate extent here - it read close to
        // or at the 200px cap regardless of actual item count, showing a fixed/oversized popup
        // with empty space below the last real item instead of sizing to content.
        height: Math.min(root.count * 26, 200)
        padding: 0
        margins: 0

        // Small slide+fade on open/close (150-250ms per the app's motion spec - see
        // NebulaTheme.qml). "from: popup.y - 8" / "to: popup.y" both work off popup.y's *final*
        // value, not a relative offset computed live - onAboutToShow above already set popup.y to
        // its real open-state position before this transition ever starts (enter plays as the
        // popup is shown, after onAboutToShow has run), so this always slides in from 8px above
        // wherever the popup actually belongs, never from some stale previous position.
        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: NebulaTheme.durationSlow; easing.type: Easing.OutCubic }
            NumberAnimation { property: "y"; from: popup.y - 8; to: popup.y; duration: NebulaTheme.durationSlow; easing.type: Easing.OutCubic }
        }
        exit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: NebulaTheme.durationNormal; easing.type: Easing.InCubic }
            NumberAnimation { property: "y"; from: popup.y; to: popup.y - 8; duration: NebulaTheme.durationNormal; easing.type: Easing.InCubic }
        }

        // Positions in Overlay.overlay's coordinate space, computed at open time - the point of
        // parenting there (see this file's header comment) is that it's detached from root's own
        // layout/coordinate space, so plain anchoring to root can't reach across that; explicit
        // x/y via mapToItem stands in for what anchors.top: root.bottom would have been.
        onAboutToShow: {
            const pos = root.mapToItem(popup.parent, 0, root.height);
            popup.x = pos.x;
            popup.y = pos.y;
        }

        // Explicitly anchored to the popup's own real bounds (matching Qt's own Popup
        // customization examples, which always do this) rather than left with no sizing/anchors
        // of its own - unanchored, it fell back to some other implicit size instead of reliably
        // matching popup.width/height, so the visible border (drawn by this Rectangle) didn't
        // match the popup's actual paint/hit-test extent (width/height above).
        background: Rectangle {
            anchors.fill: parent
            radius: NebulaTheme.radiusS
            color: NebulaTheme.withAlpha(NebulaTheme.bgElevated, NebulaTheme.panelAlphaElevated)
            border.width: 1
            border.color: NebulaTheme.borderColorStrong
        }

        // Same reasoning as background above - anchors.fill: parent (parent here being popup's
        // own content area) instead of separately-read width/height, so there's exactly one
        // source of truth for this popup's real bounds, not two properties that happen to (or
        // might not) agree.
        contentItem: ListView {
            id: listView
            anchors.fill: parent
            clip: true
            model: root.model
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                width: listView.width
                height: 26
                radius: NebulaTheme.radiusS
                color: optionArea.containsMouse ? NebulaTheme.rowHover : "transparent"
                Behavior on color { ColorAnimation { duration: NebulaTheme.durationFast } }

                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: NebulaTheme.spacingS
                    color: NebulaTheme.textPrimary
                    text: modelData
                }

                MouseArea {
                    id: optionArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        // Deliberately not touching root.currentIndex here - every call site
                        // binds it declaratively (e.g. currentIndex: bridge.profileNames.
                        // indexOf(bridge.currentProfile)), the same non-self-mutating pattern as
                        // CaoCheckBox/CaoTextField elsewhere in this app: assigning to a property
                        // that also has an external binding permanently destroys that binding.
                        // activated() only relays which index was picked; the bridge that owns
                        // the real selection state flows the new currentIndex back in once it's
                        // actually changed.
                        root.activated(index);
                        popup.close();
                    }
                }
            }
        }
    }
}
