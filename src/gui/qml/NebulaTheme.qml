// Centralized "Nebula" design tokens - every color, spacing, radius, and animation duration used
// by the Cao* shared controls and the app's other QML surfaces lives here, once, instead of
// repeated as literal hex strings in each file (the app's original convention, and how it grew
// into a dozen-plus files each hand-carrying the same handful of purple hexes with no single
// source of truth for what the palette actually was). Referenced as `NebulaTheme.accentPrimary`
// etc. from any .qml file in this same directory - `singleton NebulaTheme 1.0 NebulaTheme.qml` in
// this directory's qmldir file is what makes that work without an explicit import statement,
// exactly like CaoButton/CaoComboBox/etc. are already usable across files with no import: QML
// treats a file's own directory as an implicit import, and a qmldir singleton declaration in that
// directory applies to the implicit import too, not just an explicit versioned one.
//
// Palette, spacing scale, and motion timings below are a fixed design spec (deep space / nebula
// clouds / premium desktop tooling - not a gamer-RGB or cyberpunk-neon look), not values picked
// file-by-file - see each section for what it's for.
pragma Singleton
import QtQuick

QtObject {
    id: theme

    // ==================== Backgrounds ====================
    // bgDeep is the void behind everything (dialog fallback fill, scrim over the nebula photo).
    // bgPanel is the standard translucent surface color for panels/boxes/controls. bgElevated is
    // for surfaces that should read as "above" a panel - TopBar's header strip, CaoGroupBox's
    // nested content box sitting inside a module's own outer contentBox. bgInput is darker still,
    // for text entry - inputs need to read as a "well" sunk into the panel around them, not a
    // panel in their own right (see the Text Inputs section of the design spec this implements).
    readonly property color bgDeep: "#0B0618"
    readonly property color bgPanel: "#120824"
    readonly property color bgElevated: "#1A0F33"
    readonly property color bgInput: "#0B0618"

    // Panels are translucent surfaces over the nebula backdrop (see NebulaBackground.qml/
    // NebulaDialogBackground.qml). Deliberately bolder than the original 0.92/0.96 spec value -
    // updated direction is "photo clearly visible" everywhere, not just a faint texture behind
    // controls, while staying high enough that white/light text keeps working against whatever's
    // behind it. withAlpha() below is how call sites turn bgPanel/bgElevated/bgDeep into an
    // actual translucent Rectangle color at a given opacity instead of hand-writing
    // "#8C120824"-style literals at every call site.
    readonly property real panelAlpha: 0.55
    readonly property real panelAlphaElevated: 0.65

    function withAlpha(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha);
    }

    // ==================== Accents ====================
    // Purple is the primary interactive accent (default borders, checked states, primary button
    // fill). Pink is secondary (used sparingly - a second accent for variety, not a competing
    // primary). Cyan is reserved specifically for focus/highlight - the one state that should
    // never be ambiguous with a plain hover, so it's the one color nothing else in this palette
    // borrows. Blue is CaoCheckBox's own checked-fill color specifically (paired with a pink
    // border - see that file) - sampled from the same photo family as borderColor below rather
    // than picked independently, so it still reads as "part of this palette," not a fourth,
    // unrelated hue.
    readonly property color accentPrimary: "#B84DFF"
    readonly property color accentSecondary: "#FF5CB8"
    readonly property color accentHighlight: "#55D6FF"
    readonly property color accentBlue: "#4D8DFF"

    // ==================== Text ====================
    readonly property color textPrimary: "#F2F2F7"
    readonly property color textSecondary: "#CFCFE8"
    readonly property color textDisabled: "#666680"

    // ==================== Borders ====================
    // Sampled from the actual nebula photos (both galaxy.jpg and galaxy2.jpg - see
    // NebulaBackground.qml/NebulaDialogBackground.qml), not the flat accentPrimary purple this
    // used before: #7B5EF0 is blue_arm/deep_blue (galaxy2's spiral) blended with violet_mid/
    // blue_teal (galaxy.jpg) and accentPrimary itself, so borders read as "drawn from the actual
    // images" rather than a UI-picked color that happens to be in the same family. Written as
    // #AARRGGBB (alpha first) since QML color strings take alpha that way, not as a separate
    // rgba()/Qt.rgba() call, for a value used this often across every bordered surface in the app.
    readonly property color borderColor: "#407B5EF0"
    // Hover warms toward magenta (#BD5DD4 - the blue-purple base above blended with
    // accentSecondary/galaxy2's magenta filaments) rather than just brightening the same blue -
    // both of the photos' dominant hues show up somewhere in the border language this way, not
    // just one.
    readonly property color borderColorStrong: "#8CBD5DD4"
    readonly property color borderColorFocus: accentHighlight
    // Nested/secondary panels (CaoGroupBox's inner content box, sitting inside a module's own
    // outer contentBox) lean magenta instead of repeating the same blue-purple border as their
    // parent - visible layering, and both photos' dominant hues end up represented somewhere in
    // the UI rather than only blue-purple.
    readonly property color borderColorAccent: "#40FF5CB8"
    // Hover-strength version of the above - CaoCheckBox's own border (blue fill, pink border -
    // see that file) uses this pair instead of borderColor/borderColorStrong, since a blue fill
    // reads better against a pink border than against blue-purple.
    readonly property color borderColorAccentStrong: "#80FF5CB8"

    // ==================== Radius ====================
    readonly property real radiusS: 6
    readonly property real radiusM: 8
    readonly property real radiusL: 10

    // ==================== Spacing ====================
    // A single scale every control/layout margin and spacing value should come from, instead of
    // ad-hoc numbers (6 here, 8 there, 12 somewhere else) that happened to be close enough.
    readonly property real spacingXS: 4
    readonly property real spacingS: 8
    readonly property real spacingM: 12
    readonly property real spacingL: 16
    readonly property real spacingXL: 24

    // ==================== Control sizing ====================
    readonly property real controlHeight: 26
    readonly property real indicatorSize: 16

    // ==================== Motion ====================
    // Subtle only, per spec: hover/focus/tab transitions, 150-250ms, nothing dramatic. Three
    // tiers rather than one fixed number - fast for the small stuff (checkbox fill, button
    // gradient), normal for borders/text color, slow for the few larger movements (dropdown
    // slide - see CaoComboBox.qml).
    readonly property int durationFast: 150
    readonly property int durationNormal: 200
    readonly property int durationSlow: 250

    // ==================== Buttons ====================
    // Dark purple vertical gradient at rest, brighter on hover, darker (flatter) when pressed -
    // see the Buttons section of the design spec. Focus is a cyan border, not a glow (spec calls
    // for a "cyan outline or glow" but separately says to avoid large glow effects throughout -
    // an outline satisfies both).
    readonly property color buttonTopNormal: "#241640"
    readonly property color buttonBottomNormal: "#150A2A"
    readonly property color buttonTopHover: "#331F5E"
    readonly property color buttonBottomHover: "#1D0F3A"
    readonly property color buttonTopPressed: "#170B2E"
    readonly property color buttonBottomPressed: "#0D051C"

    // ==================== List/row hover ====================
    // Dropdown items, list rows - a soft accent wash rather than a hard color swap.
    readonly property color rowHover: "#26B84DFF"
}
