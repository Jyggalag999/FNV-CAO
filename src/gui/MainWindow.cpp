/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainWindow.hpp"

#include "AdvancedAnimationsModule.hpp"
#include "AdvancedBSAModule.hpp"
#include "AdvancedMeshesModule.hpp"
#include "AdvancedTexturesModule.hpp"
#include "GeneralBSAModule.hpp"
#include "LevelSelector.hpp"
#include "PatternsManagerWindow.hpp"
#include "ProfilesManagerWindow.hpp"
#include "SelectGPUWindow.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "ui_MainWindow.h"
#include "utils/utils.hpp"

#include <plog/Log.h>

#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QTimer>
#include <QUrl>

namespace cao {
// k_nexus_url used to live here too, for the now-removed Help menu's "Documentation" link -
// removed with that menu (see MainWindow.ui). discord/kofi stay: first_start() below (currently
// unused - see its own call site comment) still references them.
constexpr static inline auto k_discord_url = "https://discord.gg/SwfTzHGQcy";
constexpr static inline auto k_kofi_url    = "https://ko-fi.com/guekka";

auto get_dark_style_sheet() noexcept -> QString
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        PLOG_ERROR << "Cannot set dark style";
        return {};
    }
    QString base = f.readAll();

    // qdarkstyle's own palette (documented in its header comment) repeats these same ~8 hex
    // colors across ~200 selectors spanning the whole file - borders, disabled states, hover/
    // pressed/selected accents, scrollbars, menu items, tab variants for all four edges, etc.
    // nebula_overrides below re-themes the widgets this app actually shows, but several of those
    // rules kept losing to *more specific* base selectors it didn't know to match (QTabBar::tab:top
    // beats QTabBar::tab, QScrollBar::handle:vertical beats QScrollBar::handle, QComboBox
    // QAbstractItemView's own colors get overridden by QComboBox's, etc.) - which is exactly why
    // gray-blue borders/fills/highlights kept showing through everywhere. Recoloring the base
    // text in place, before any override rules even get layered on, fixes every one of those
    // selectors at the source and sidesteps the specificity game entirely - anything still
    // rendering qdarkstyle's literal color values now renders this app's instead.
    // Fixed design-spec palette (see NebulaTheme.qml, the QML-side equivalent of every color
    // below - both need to match, since this app has native QWidget UI *and* QML UI sharing the
    // same window). QSS can't reference that singleton directly (it's a Qt Quick concept, not a
    // widget stylesheet one), so the same hex values are hand-kept in sync here.
    base.replace(QStringLiteral("#32414B"), QStringLiteral("#2C1E57"), Qt::CaseInsensitive); // borders (most uses), some fills - see NebulaTheme.qml's borderColor comment
    base.replace(QStringLiteral("#19232D"), QStringLiteral("#120824"), Qt::CaseInsensitive); // dark fills (bgPanel)
    base.replace(QStringLiteral("#F0F0F0"), QStringLiteral("#F2F2F7"), Qt::CaseInsensitive); // light/white text (textPrimary)
    base.replace(QStringLiteral("#148CD2"), QStringLiteral("#B84DFF"), Qt::CaseInsensitive); // hover/selection accent (accentPrimary)
    base.replace(QStringLiteral("#787878"), QStringLiteral("#666680"), Qt::CaseInsensitive); // disabled gray text/fills (textDisabled)
    base.replace(QStringLiteral("#1464A0"), QStringLiteral("#7A2FC4"), Qt::CaseInsensitive); // selected accent (darker accentPrimary)
    base.replace(QStringLiteral("#14506E"), QStringLiteral("#2A1C42"), Qt::CaseInsensitive); // dim (disabled+selected)
    base.replace(QStringLiteral("#505F69"), QStringLiteral("#3A2456"), Qt::CaseInsensitive); // lighter fill

    // "Nebula" design spec: deep space background, purple primary / pink secondary / cyan
    // highlight accents, translucent panels over the nebula photo (NebulaBackground.qml/
    // NebulaDialogBackground.qml) rather than flat opaque fills. Layered after the qdarkstyle
    // base so its widget-chrome icons/arrows keep working; later rules win on equal specificity.
    static const QString nebula_overrides = R"(
        /* Base qdarkstyle's own most-generic rule (QWidget { background-color: #19232D; ... })
           sets an opaque fill for literally every widget in the app that nothing more specific
           overrides - base.replace() above only recolors its hex value (to #120824), it doesn't
           remove it. That was invisible for as long as every widget either had its own specific
           background rule here or was itself covered by opaque QML content - once GeneralBSAModule
           .qml and friends' contentBox/CaoGroupBox went from a translucent Rectangle to a plain
           empty Item (removing the "boxes" - see those files), the one native widget that was
           never given its own specific rule (the plain QWidget QTabWidget uses internally to host
           each tab's page) stopped being hidden behind that QML content and started showing this
           generic opaque fill instead. Explicit transparent background here is what actually
           fixes it, for that widget and any other plain QWidget in the same situation. */
        QWidget {
            background-color: transparent;
            color: #F2F2F7;
        }
        QWidget:disabled {
            background-color: transparent;
            color: #666680;
        }
        #centralwidget {
            background-color: transparent;
        }
        QMainWindow {
            background-color: transparent;
        }
        QDialog {
            background-color: #0B0618;
        }
        /* No fill, no border - was a translucent Rectangle-equivalent, removed per feedback that
           the stacked boxes (this one, QTabWidget::pane further out, CaoGroupBox.qml's own nested
           box on the QML side) made the UI read as boxes-on-boxes over the nebula rather than the
           nebula itself being the focus. nebula_background_widget_ (galaxy2.jpg - see
           NebulaBackground.qml) now shows straight through mainGroupBox with nothing to block it. */
        QGroupBox {
            background-color: transparent;
            border: none;
            color: #F2F2F7;
        }
        /* Blue, not purple - matches CaoGroupBox.qml's own title text (see that file's comment):
           purple text over this app's purple/magenta nebula backdrop read as low-contrast/hard
           to read. */
        QGroupBox::title {
            color: #4D8DFF;
        }
        QGroupBox::indicator {
            width: 12px;
            height: 12px;
        }
        /* Dark purple gradient at rest, brighter on hover, darker (flatter) when pressed - see
           the design spec's Buttons section. Matches CaoButton.qml's own gradient exactly (same
           treatment on both the QML and native-widget buttons, since this app still has both). */
        QPushButton {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                               stop:0 #241640, stop:1 #150A2A);
            border: 1px solid rgba(123, 94, 240, 64);
            border-radius: 6px;
            color: #F2F2F7;
        }
        QPushButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                               stop:0 #331F5E, stop:1 #1D0F3A);
            border: 1px solid rgba(189, 93, 212, 128);
        }
        QPushButton:pressed {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                               stop:0 #170B2E, stop:1 #0D051C);
            border: 1px solid rgba(123, 94, 240, 64);
        }
        /* Cyan focus outline, not a glow - see the design spec's note to avoid large glow
           effects everywhere while still calling for a "cyan outline or glow" on focus. */
        QPushButton:focus {
            border: 2px solid #55D6FF;
        }
        QPushButton:disabled {
            background-color: #1A0F33;
            border: 1px solid rgba(123, 94, 240, 32);
            color: #666680;
        }
        /* Run and Open Directory (mainGroupBox's two primary actions) - tried a nebula photo
           crop here first (background-image), reverted per feedback: translucent like every
           other panel instead (revealing the nebula behind them the same way mainGroupBox's own
           QGroupBox background does), standing out via a distinct accent-colored bold label
           rather than imagery. Pink (accentSecondary), not purple or cyan - purple's already the
           default border/accent everywhere on every other button, and cyan is reserved
           specifically for focus outlines throughout this app (see NebulaTheme.qml's
           borderColorFocus convention) - pink is the one accent hue not already carrying a
           different meaning elsewhere, so using it here doesn't clash with either. */
        QPushButton#processButton, QPushButton#userPathButton {
            background-color: rgba(26, 15, 51, 140);
            border: 1px solid rgba(255, 92, 184, 100);
            color: #FF5CB8;
            font-weight: 600;
        }
        QPushButton#processButton:hover, QPushButton#userPathButton:hover {
            background-color: rgba(26, 15, 51, 180);
            border: 1px solid rgba(255, 92, 184, 160);
            color: #FF8FCB;
        }
        QPushButton#processButton:pressed, QPushButton#userPathButton:pressed {
            background-color: rgba(11, 6, 24, 160);
            border: 1px solid rgba(255, 92, 184, 100);
            color: #FF5CB8;
        }
        /* Inputs are darker than the panels around them (bgInput, not bgPanel - see
           NebulaTheme.qml) - they should read as a well sunk into the surrounding panel, not
           another panel of their own. */
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #0B0618;
            border: 1px solid rgba(123, 94, 240, 64);
            border-radius: 6px;
            color: #F2F2F7;
            selection-background-color: #55D6FF;
            selection-color: #0B0618;
        }
        QSpinBox, QComboBox {
            background-color: #0B0618;
            border: 1px solid rgba(123, 94, 240, 64);
            border-radius: 6px;
            color: #F2F2F7;
            selection-background-color: #55D6FF;
        }
        QSpinBox:disabled, QComboBox:disabled {
            background-color: #1A0F33;
            border: 1px solid rgba(123, 94, 240, 32);
            color: #666680;
        }
        QComboBox::drop-down {
            border-left: 1px solid rgba(123, 94, 240, 64);
        }
        QComboBox QAbstractItemView {
            background-color: rgba(26, 15, 51, 166);
            border: 1px solid rgba(189, 93, 212, 128);
            border-radius: 8px;
            color: #F2F2F7;
            selection-background-color: rgba(184, 77, 255, 38);
            selection-color: #F2F2F7;
        }
        /* Cyan focus outline, per the design spec's Text Inputs/Dropdowns sections - the one
           state that should never be ambiguous with a plain hover border. */
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
            border: 2px solid #55D6FF;
        }
        QCheckBox, QRadioButton {
            background-color: transparent;
            color: #F2F2F7;
        }
        QRadioButton:disabled {
            background-color: transparent;
            color: #666680;
        }
        /* QCheckBox/QRadioButton/QMenu's checkable-item indicators are qdarkstyle bitmap icons
           (checkbox_checked.png etc.) with the gray-blue palette baked into the pixels - QSS text
           substitution can't recolor an image. Overriding the indicator subcontrols with plain
           background-color/border (no image) replaces those icons with a flat swatch instead,
           matching the app's own hand-rolled CaoCheckBox.qml (dark fill, purple border, filled
           purple square when checked). Base styles these through two-pseudo-state selectors
           (QCheckBox::indicator:unchecked:hover, etc.), so matching selectors are needed here too
           - a single-pseudo-state QCheckBox::indicator:hover would lose to them, same as the
           QTabBar::tab:top/QScrollBar::handle:vertical specificity issue elsewhere in this sheet.
           `image` is also a separate property from `background-color`/`border` - the base icon
           still drew on *top* of a plain background-color override otherwise (as a literal blue
           checkmark glyph sitting over a correctly-purple square), so every rule below explicitly
           clears it. */
        /* Pink border, blue checked-fill - CaoCheckBox.qml (the QML equivalent) uses the same
           pair, deliberately different from every other bordered control's blue-purple, so this
           native QCheckBox (Dry run) matches its QML counterparts instead of standing out as the
           one checkbox still on the old scheme. */
        QCheckBox::indicator:unchecked, QCheckBox::indicator:indeterminate {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #0B0618;
            border: 1px solid rgba(255, 92, 184, 64);
            border-radius: 3px;
        }
        QCheckBox::indicator:unchecked:hover, QCheckBox::indicator:unchecked:focus,
        QCheckBox::indicator:unchecked:pressed,
        QCheckBox::indicator:indeterminate:hover, QCheckBox::indicator:indeterminate:focus,
        QCheckBox::indicator:indeterminate:pressed {
            image: none;
            border: 1px solid rgba(255, 92, 184, 128);
        }
        QCheckBox::indicator:unchecked:disabled, QCheckBox::indicator:indeterminate:disabled {
            image: none;
            background-color: #1A0F33;
            border: 1px solid rgba(255, 92, 184, 32);
        }
        QCheckBox::indicator:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #4D8DFF;
            border: 1px solid #4D8DFF;
            border-radius: 3px;
        }
        QCheckBox::indicator:checked:hover, QCheckBox::indicator:checked:focus,
        QCheckBox::indicator:checked:pressed {
            image: none;
            background-color: #71A4FF;
            border: 1px solid #71A4FF;
        }
        QCheckBox::indicator:checked:disabled {
            image: none;
            background-color: #666680;
            border: 1px solid rgba(255, 92, 184, 32);
        }
        QRadioButton::indicator:unchecked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #0B0618;
            border: 1px solid rgba(123, 94, 240, 64);
            border-radius: 6px;
        }
        QRadioButton::indicator:unchecked:hover, QRadioButton::indicator:unchecked:focus,
        QRadioButton::indicator:unchecked:pressed {
            image: none;
            border: 1px solid rgba(189, 93, 212, 128);
        }
    )"
    // Split into a second adjacent string literal here purely to stay under MSVC's per-literal
    // size limit (error C2026, "string too big") - two adjacent string literals concatenate into
    // one at compile time, same result as if this were still a single raw string. No functional
    // difference, just where the C++ source happens to break.
    R"(
        QRadioButton::indicator:unchecked:disabled {
            image: none;
            background-color: #1A0F33;
            border: 1px solid rgba(123, 94, 240, 32);
        }
        QRadioButton::indicator:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #0B0618;
            border: 3px solid #B84DFF;
            border-radius: 6px;
        }
        QRadioButton::indicator:checked:hover, QRadioButton::indicator:checked:focus,
        QRadioButton::indicator:checked:pressed {
            image: none;
            border: 3px solid #C875FF;
        }
        QRadioButton::indicator:checked:disabled {
            image: none;
            background-color: #1A0F33;
            border: 3px solid rgba(123, 94, 240, 32);
        }
        /* Same pink-border/blue-fill checkbox scheme as QCheckBox::indicator above - "Enable dark
           theme" in the Options menu is a checkable QAction, styled through this selector rather
           than QCheckBox's. */
        QMenu::indicator:non-exclusive:unchecked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #0B0618;
            border: 1px solid rgba(255, 92, 184, 64);
            border-radius: 3px;
        }
        QMenu::indicator:non-exclusive:unchecked:selected {
            image: none;
            border: 1px solid rgba(255, 92, 184, 128);
        }
        QMenu::indicator:non-exclusive:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #4D8DFF;
            border: 1px solid #4D8DFF;
            border-radius: 3px;
        }
        QMenu::indicator:non-exclusive:checked:selected {
            image: none;
            background-color: #71A4FF;
            border: 1px solid #71A4FF;
        }
        QMenu::indicator:exclusive:unchecked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #0B0618;
            border: 1px solid rgba(123, 94, 240, 64);
            border-radius: 6px;
        }
        QMenu::indicator:exclusive:unchecked:selected {
            image: none;
            border: 1px solid rgba(189, 93, 212, 128);
        }
        QMenu::indicator:exclusive:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #0B0618;
            border: 3px solid #B84DFF;
            border-radius: 6px;
        }
        QMenu::indicator:exclusive:checked:selected {
            image: none;
            border: 3px solid #C875FF;
        }
        /* No fill, no border - was the single largest translucent-box surface in the app (every
           tab's full page); removed for the same reason as QGroupBox above. nebula_background_widget_
           (galaxy2.jpg - see NebulaBackground.qml) now shows straight through with nothing here to
           block it. */
        QTabWidget::pane {
            border: none;
            background-color: transparent;
        }
        /* Base qdarkstyle only really styles tabs through its :top/:bottom/:left/:right variants
           (e.g. QTabBar::tab:top, QTabBar::tab:top:selected) - those are more specific than a
           plain QTabBar::tab/:selected/:hover (extra pseudo-state), so they always won regardless
           of cascade order, leaving tabs on qdarkstyle's own gray-blue fill with a blue selected/
           hover accent. Matching the same :top selectors here (tabs only ever appear on top in
           this app) recolors them without touching the shape/spacing properties (padding,
           margins, corner radius) those base rules also set, which are left alone.
           No background box on any tab, selected or not (same "no boxes" direction as
           QGroupBox/QTabWidget::pane above) - which tab is active reads purely from text color
           plus an accent underline (border-bottom), not a filled rectangle. */
        QTabBar::tab:top {
            background-color: transparent;
            color: #CFCFE8;
            border: none;
            border-bottom: 2px solid transparent;
        }
        QTabBar::tab:top:selected {
            background-color: transparent;
            color: #F2F2F7;
            border: none;
            border-bottom: 2px solid #B84DFF;
        }
        QTabBar::tab:top:!selected:hover {
            background-color: transparent;
            color: #F2F2F7;
            border: none;
            border-bottom: 2px solid rgba(184, 77, 255, 100);
        }
        QListWidget, QTreeWidget, QTableWidget {
            background-color: rgba(11, 6, 24, 200);
            border: 1px solid rgba(123, 94, 240, 64);
            border-radius: 6px;
            alternate-background-color: rgba(18, 8, 36, 200);
        }
        QListWidget::item:selected, QTreeWidget::item:selected, QTableWidget::item:selected {
            background-color: rgba(184, 77, 255, 90);
            color: #F2F2F7;
        }
        QListWidget::item:hover, QTreeWidget::item:hover {
            background-color: rgba(184, 77, 255, 38);
        }
        QProgressBar {
            border: 1px solid rgba(189, 93, 212, 128);
            border-radius: 6px;
            background-color: #0B0618;
            text-align: center;
            color: #F2F2F7;
        }
        QProgressBar::chunk {
            background-color: #B84DFF;
            border-radius: 5px;
        }
        /* Base qdarkstyle's QMenuBar rule (not just its own default palette - the selector
           itself) sets a 1px border and 2px padding, which is what actually drew the visible box
           around "Options" - overriding background-color alone (the only property nebula_overrides
           touched before) left that border/padding in place. Explicitly clearing all three plus
           the :focus border below is what gets floating colored text with no box, at rest or
           focused - the item itself only gets a background at all when selected/pressed, and even
           then it's a soft rounded highlight, not a hard-edged box. */
        QMenuBar {
            background-color: transparent;
            border: none;
            padding: 4px;
            color: #F2F2F7;
        }
        QMenuBar:focus {
            border: none;
        }
        QMenuBar::item {
            background: transparent;
            border: none;
            border-radius: 4px;
        }
        QMenuBar::item:selected {
            background-color: rgba(184, 77, 255, 38);
            border: none;
        }
        QMenuBar::item:pressed {
            background-color: rgba(184, 77, 255, 60);
            border: none;
        }
        QMenu {
            background-color: rgba(26, 15, 51, 166);
            border: 1px solid rgba(189, 93, 212, 128);
            border-radius: 8px;
            color: #F2F2F7;
        }
        QMenu::item:selected {
            background-color: rgba(184, 77, 255, 60);
            color: #F2F2F7;
        }
        QScrollBar:vertical, QScrollBar:horizontal {
            background-color: #0B0618;
        }
        QScrollBar::handle {
            background-color: rgba(184, 77, 255, 100);
            border-radius: 4px;
        }
        QScrollBar::handle:hover {
            background-color: #B84DFF;
        }
        /* Cyan border - was gold (#d97a2d), a color with no place in this palette anymore (the
           old nebula photo's rim-light gold; the current palette has no gold/orange at all).
           Cyan reads as "informational," which fits a tooltip better than either other accent. */
        QToolTip {
            background-color: rgba(26, 15, 51, 245);
            border: 1px solid #55D6FF;
            border-radius: 6px;
            color: #F2F2F7;
        }
        QLabel {
            color: #F2F2F7;
        }
    )";

    return base + nebula_overrides;
}

auto set_theme(GuiTheme theme) noexcept -> bool
{
    if (theme == GuiTheme::Light)
    {
        qApp->setStyleSheet("");
    }
    else if (theme == GuiTheme::Dark)
    {
        static const QString dark_sheet = get_dark_style_sheet();

        if (dark_sheet.isEmpty())
            return false;

        qApp->setStyleSheet(dark_sheet);
    }
    return true;
}

// Step 5: pattern-row and Manage-Profiles-button visibility (formerly toggled here via
// set_patterns_enabled()/ui.manageProfiles->setHidden()) are now computed live by
// TopBarBridge::patternsVisible()/manageProfilesVisible() straight from settings.gui.gui_mode -
// nothing to push into the (now QML) top bar from here anymore.
void set_gui_level(ModuleDisplay &modules, const Settings &settings) noexcept
{
    modules.clear_modules();

    switch (settings.gui.gui_mode)
    {
        case GuiMode::QuickOptimize:
        {
            break;
        }
        case GuiMode::Medium:
        {
            modules.add_module(std::make_unique<GeneralBSAModule>());
            modules.add_module(std::make_unique<AdvancedBSAModule>());
            modules.add_module(std::make_unique<AdvancedTexturesModule>());
            break;
        }
        case GuiMode::Advanced:
        {
            modules.add_module(std::make_unique<GeneralBSAModule>());
            modules.add_module(std::make_unique<AdvancedBSAModule>());
            modules.add_module(std::make_unique<AdvancedMeshesModule>());
            modules.add_module(std::make_unique<AdvancedTexturesModule>());
            modules.add_module(std::make_unique<AdvancedAnimationsModule>());
            break;
        }
    }

    // Deliberately not calling modules.hide_unsupported() here anymore: it used to hide a tab
    // entirely for any module unsupported by the current profile's game (e.g. Meshes/Animations
    // tabs vanishing outright), which also hid that the feature existed at all. Each module's own
    // IWindowModule::setup() already disables itself (setDisabled(true), skipping
    // settings_to_ui()) when unsupported - visible-but-inert, not gone - which is what's wanted:
    // an unsupported tab's toggles are still there, they just can't do anything.
}

void ui_to_settings(const Ui::MainWindow &ui,
                    const TopBarBridge &top_bar_bridge,
                    const ModuleDisplay &module_display,
                    Settings &settings)
{
    settings.gui.gui_theme = ui.actionEnableDarkTheme->isChecked() ? GuiTheme::Dark : GuiTheme::Light;

    switch (settings.gui.gui_mode)
    {
        case GuiMode::QuickOptimize:
        {
            // Step 5: profiles is QML now (TopBar.qml) - top_bar_bridge_.currentProfile() is the
            // same underlying value the old ui.profiles->currentText() read, since both are
            // ultimately driven by settings.current_profile_name() (see settings_to_ui() before
            // this change, and TopBarBridge::currentProfile() now).
            if (top_bar_bridge.currentProfile() == "SLE")
                settings.current_profile() = Profile::make_base(btu::Game::SLE);
            else if (top_bar_bridge.currentProfile() == "SSE")
                settings.current_profile() = Profile::make_base(btu::Game::SSE);
            else
                throw UiException("Invalid profile selected");
            break;
        }
        case GuiMode::Medium:
        case GuiMode::Advanced: break; // nothing to do
    }

    settings.current_profile().input_path        = ui.inputDirTextEdit->text().toStdString();
    settings.current_profile().dry_run           = ui.dryRunCheckBox->isChecked();
    settings.current_profile().mods_blacklist    = {}; // TODO
    settings.current_profile().optimization_mode = ui.modeChooserComboBox->currentData()
                                                       .value<OptimizationMode>();

    // has to be last because may rely on GuiSettings being already filled
    for (const auto *module : module_display.get_modules())
        module->ui_to_settings(settings);

} // namespace cao

void settings_to_ui(const Settings &settings, Ui::MainWindow &ui, ModuleDisplay &module_display) noexcept
{
    set_theme(settings.gui.gui_theme);

    // Cache current index to keep selected tab if possible.
    const auto old_tab_index = module_display.current_index();

    set_gui_level(module_display, settings);

    ui.inputDirTextEdit->setText(to_qstring(settings.current_profile().input_path.u8string()));
    ui.dryRunCheckBox->setChecked(settings.current_profile().dry_run);
    [[maybe_unused]] const bool success
        = select_data(*ui.modeChooserComboBox, settings.current_profile().optimization_mode);
    assert(success);

    // Step 5: profile/pattern list population and current-selection sync (formerly set_items()/
    // select_text() calls against ui.profiles/ui.patterns here) are now just TopBarBridge's
    // Q_PROPERTY getters, computed live from settings whenever QML re-reads them after
    // top_bar_bridge_.refresh() (see MainWindow::refresh_ui()) - nothing to push from here.

    for (auto *module : module_display.get_modules())
        module->setup(settings);

    module_display.set_current_index(old_tab_index);
}

void first_start(bool &first_run) noexcept
{
    if (!first_run)
        return;

    constexpr auto welcome
        = R"(It appears you are running CAO for the first time. All options have tooltips explaining what
          they do. If you need help, you can also <a href="%1">join us on Discord</a>.<br>
          <br>
          If you like my work, <a href="%2">please consider supporting me</a>.<br>
          Thanks for using CAO!)";

    first_run = false;
    QMessageBox box(QMessageBox::Information,
                    QObject::tr("Welcome to %1 %2")
                        .arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
                    QObject::tr(welcome).arg(k_discord_url, k_kofi_url));
    box.setTextFormat(Qt::TextFormat::RichText);
    box.exec();
}

MainWindow::MainWindow(Settings settings, QWidget *parent)
    : QMainWindow(parent)
    , settings_(std::move(settings))
    , ui_(std::make_unique<Ui::MainWindow>())
    , top_bar_bridge_(settings_)
{
    ui_->setupUi(this);
    module_display_.set_tab_widget(ui_->tabWidget);

    // Central widget fully repaints its own region regardless of QMainWindow's own paintEvent,
    // so the nebula background has to track it directly via an event filter, not `this` - see
    // eventFilter()'s QEvent::Resize handling below. topBarContainer is also watched, to keep
    // top_bar_widget_'s geometry (also not layout-managed - see its own declaration) in sync with
    // its reserved placeholder space as the window resizes.
    ui_->centralwidget->installEventFilter(this);
    ui_->topBarContainer->installEventFilter(this);

    // Step 5: nebula background, parented directly to `this` (not centralwidget - see this
    // member's declaration in MainWindow.hpp for why) and not added to any layout, so it can sit
    // behind the real layout-managed widgets rather than taking its own row. lower() pushes it to
    // the back of the stacking order now, right after construction, since it would otherwise be
    // topmost (last child added wins the top of the stack in Qt).
    nebula_background_widget_ = new QQuickWidget(this);
    nebula_background_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    nebula_background_widget_->setSource(QUrl("qrc:/qml/NebulaBackground.qml"));
    sync_nebula_background_geometry();
    nebula_background_widget_->lower();

    // Step 5: real top bar, replacing the old native profiles/patterns QGroupBox. topBarContainer
    // stays in centralwidget's layout as a fixed-height placeholder (76px - covers both rows: 2 *
    // 26px content + 8px row spacing + 8px top/bottom margins, see TopBar.qml) purely to reserve
    // the right amount of space for the closed state; top_bar_widget_ itself is parented directly
    // to centralwidget instead of into topBarContainer's layout - like nebula_background_widget_
    // above, not layout-managed - and manually kept aligned with topBarContainer's position/width
    // (see sync_top_bar_widget_geometry(), called from eventFilter()'s QEvent::Resize handling
    // below). That split is what lets it grow taller than topBarContainer's reserved 76px while a
    // popup is open (see on_top_bar_popup_open_changed()) without pushing mainGroupBox/tabWidget
    // down - it isn't part of the layout at all, so growing it can only ever overlap whatever's
    // below (which raise() ensures renders on top of, correctly), never displace it.
    ui_->topBarContainer->setFixedHeight(76);

    top_bar_widget_ = new QQuickWidget(ui_->centralwidget);
    top_bar_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // Growing this widget to 300px while a dropdown is open (see sync_top_bar_widget_geometry())
    // only reserves *room* for the popup - it isn't meant to visually cover the tab content that
    // now sits under that extra height. Without these two lines, QQuickWidget paints its own
    // opaque clear color (white by default) across its *entire* rect every frame, on top of
    // whatever's beneath, regardless of what TopBar.qml itself draws there - the popup's actual
    // background stays a small correct box, but the whole rest of the grown widget still blots
    // out mainGroupBox/tabWidget behind it in a big solid rectangle. WA_AlwaysStackOnTop +
    // a transparent clear color is Qt's documented way to let a QQuickWidget layered over sibling
    // widgets actually show them through everywhere it has no opaque QML content of its own -
    // TopBar.qml's root is transparent outside its fixed 76px header for exactly this reason.
    top_bar_widget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    top_bar_widget_->setClearColor(Qt::transparent);
    top_bar_widget_->rootContext()->setContextProperty("topBar", &top_bar_bridge_);
    top_bar_widget_->setSource(QUrl("qrc:/qml/TopBar.qml"));

    top_bar_root_ = top_bar_widget_->rootObject();
    if (top_bar_root_)
        connect(top_bar_root_, SIGNAL(anyPopupOpenChanged()), this, SLOT(on_top_bar_popup_open_changed()));

    sync_top_bar_widget_geometry();
    top_bar_widget_->raise();

    setAcceptDrops(true);

    // Setting data for widgets
    set_data(*ui_->modeChooserComboBox, tr("One mod"), OptimizationMode::SingleMod);
    set_data(*ui_->modeChooserComboBox, tr("Several mods"), OptimizationMode::SeveralMods);

    // Connecting widgets that do not depend on current profile
    connect(&top_bar_bridge_, &TopBarBridge::manageProfilesRequested, this, [this] {
        ProfilesManagerWindow profiles_manager(settings_);
        profiles_manager.exec();
        refresh_ui();
    });

    connect(&top_bar_bridge_, &TopBarBridge::managePatternsRequested, this, [this] {
        save_settings();
        PatternsManagerWindow patterns_manager(settings_);
        patterns_manager.exec();
        refresh_ui();
    });

    connect(ui_->actionSelect_GPU, &QAction::triggered, this, [this] {
        auto window = SelectGPUWindow();
        window.set_selected_index(settings_.current_profile().gpu_index);
        window.exec();

        const auto idx = window.get_selected_index();
        if (window.result() == QDialog::Accepted && idx.has_value())
            settings_.current_profile().gpu_index = idx.value();
    });

    connect(ui_->actionOpen_config_dir, &QAction::triggered, this, [this] {
        const auto dir = to_qstring(btu::fs::absolute(settings_.config_directory()).u8string());
        QDesktopServices::openUrl(QUrl("file:///" + dir));
    });

    connect(ui_->userPathButton, &QPushButton::pressed, this, [this] {
        const auto &current_path = ui_->inputDirTextEdit->text();

        const QString dir = QFileDialog::getExistingDirectory(this,
                                                              tr("Open Directory"),
                                                              current_path,
                                                              QFileDialog::ShowDirsOnly
                                                                  | QFileDialog::DontResolveSymlinks);

        if (dir.isEmpty())
            return;

        ui_->inputDirTextEdit->setText(dir);
    });

    auto &common_settings = settings_.gui;
    ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
    set_theme(common_settings.gui_theme);

    connect(ui_->actionEnableDarkTheme, &QAction::triggered, this, [this, &common_settings](bool state) {
        const GuiTheme theme      = state ? GuiTheme::Dark : GuiTheme::Light;
        common_settings.gui_theme = theme;
        ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
        set_theme(theme);
    });

    connect(&top_bar_bridge_, &TopBarBridge::profileSelected, this, [this](const QString &name) {
        if (!settings_.set_current_profile(to_u8string(name)))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not set the current profile. Please restart the application"));
        }
        // Reset selected pattern to default.
        settings_.gui.selected_pattern = k_default_pattern.text();
        refresh_ui();
    });

    connect(&top_bar_bridge_, &TopBarBridge::patternSelected, this, [this](const QString &pattern) {
        save_settings();
        settings_.gui.selected_pattern = to_u8string(pattern);
        refresh_ui();
    });

    connect(ui_->processButton, &QPushButton::pressed, this, &MainWindow::init_process);

    // Menu buttons
    connect(ui_->actionChange_level, &QAction::triggered, this, &MainWindow::run_gui_selector);

    // first_start(settings_.gui.first_run); // welcome popup disabled

    // The Level Selector dialog no longer runs at startup - always launch straight into Advanced
    // Mode instead. Still reachable afterward via the "Change level" menu action
    // (actionChange_level, connected to run_gui_selector() above) for anyone who wants a
    // different mode.
    settings_.gui.gui_mode = GuiMode::Advanced;
    refresh_ui();

    // Shrink the window's initial height to fit what refresh_ui() just populated, instead of the
    // large static default from MainWindow.ui, which left dead space below shorter tabs like BSA
    // (General).
    //
    // QMainWindow::sizeHint() turned out not to be usable for this: it apparently doesn't
    // propagate a QQuickWidget's content-driven size at all through QStackedWidget/QTabWidget/
    // centralwidget's layout chain (tried querying it both immediately and after
    // QCoreApplication::processEvents() and after a deferred 0ms QTimer post-show - all three
    // read the same too-small value, just enough for the top bar and tab bar, none of the actual
    // tab content - resize mode is SizeRootObjectToView, meaning the *view* dictates the *root's*
    // size, not the other way around, so a widget in that mode plausibly never advertises a
    // content-driven sizeHint of its own in the first place). Measuring real geometry directly
    // instead, once the window has actually been shown/laid out once (a 0ms QTimer::singleShot,
    // run right after that first show/paint - main.cpp calls show() right after this constructor
    // returns - before the user gets a chance to perceive the pre-shrink size): the current tab's
    // page widget is currently stretched to fill all the leftover vertical space QTabWidget gave
    // it, while its QML root's implicitHeight (see e.g. GeneralBSAModule.qml) reports how tall it
    // actually needs to be. Shrinking the window by exactly that difference removes the dead
    // space without needing to know or recompute anything about the chrome around it (menu bar,
    // top bar, main group box, tab bar, margins).
    //
    // Width is left as MainWindow.ui's declared default (adjustSize() would shrink that too,
    // based on the same not-usable sizeHint() - not asked for, and it broke the tab bar, forcing
    // the tab labels to overflow into a horizontal scroll arrow instead of all fitting on one
    // line).
    QTimer::singleShot(0, this, [this] {
        auto *current_tab = ui_->tabWidget->currentWidget();
        if (!current_tab)
            return;

        auto *tab_qml_widget = current_tab->findChild<QQuickWidget *>();
        if (!tab_qml_widget || !tab_qml_widget->rootObject())
            return;

        const int desired_content_height
            = static_cast<int>(tab_qml_widget->rootObject()->property("implicitHeight").toReal());
        const int height_delta           = current_tab->height() - desired_content_height;

        if (height_delta > 0)
            resize(width(), height() - height_delta);
    });
}

/// @brief Checks if the settings are valid. Displays a message box if they are not.
/// @param settings Settings to check
/// @return True if the settings are valid, false otherwise
[[nodiscard]] auto check_settings(const Settings &settings) noexcept -> bool
{
    const auto &profile = settings.current_profile();

    if (profile.input_path.empty())
    {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("The input path is empty"));
        return false;
    }

    if (!exists(profile.input_path))
    {
        QMessageBox::critical(nullptr,
                              QObject::tr("Error"),
                              QObject::tr("The input path %1 does not exist")
                                  .arg(to_qstring(profile.input_path.u8string())));
        return false;
    }

    if (profile.per_file_settings().empty())
    {
        QMessageBox::critical(nullptr,
                              QObject::tr("Error"),
                              QObject::tr("Per file settings are empty. If you don't know how to fix this "
                                          "issue, reset CAO by deleting %1")
                                  .arg(to_qstring(Settings::state_directory().u8string())));
        return false;
    }

    // TODO: refactor to increase extensibility and add more checks. The more, the better.
    return true;
}

MainWindow::~MainWindow() = default;

void MainWindow::init_process()
{
    save_settings();

    if (!check_settings(settings_))
        return;

    try
    {
        progress_window_ = std::make_unique<ProgressWindow>(
            LogReader(Settings::state_directory() / k_log_file_name));

        cao_process_ = std::jthread([this](std::stop_token &&stop_token) mutable {
            auto manager = Manager();

            connect(&manager, &Manager::files_counted, progress_window_.get(), &ProgressWindow::set_maximum);

            connect(&manager,
                    &Manager::files_processed,
                    progress_window_.get(),
                    [this](const std::filesystem::path &file, size_t count_since_last) {
                        const auto text = QString("Processing %1").arg(QString::fromStdString(file.string()));
                        progress_window_->step(std::optional(text), static_cast<int>(count_since_last));
                    });

            connect(&manager, &Manager::end, progress_window_.get(), &ProgressWindow::end);

            connect(&manager, &Manager::end, this, [this] {
                graceful_stop_dialog_.reset();
                cao_process_.reset();
                ui_->processButton->setDisabled(false);
            });

            manager.run_optimization(settings_, std::move(stop_token));
        });

        progress_window_->show();

        connect(progress_window_.get(),
                &ProgressWindow::cancelled,
                this,
                &MainWindow::stop_process_gracefully);

        ui_->processButton->setDisabled(true);
    }
    catch (const std::exception &e)
    {
        QMessageBox box(QMessageBox::Critical,
                        tr("Error"),
                        tr("An exception has been encountered and the process was forced to stop: %1")
                            .arg(e.what()));
        box.exec();
        stop_process_gracefully();
    }
}

void MainWindow::stop_process_gracefully()
{
    if (cao_process_)
    {
        cao_process_->request_stop();

        graceful_stop_dialog_ = std::make_unique<QProgressDialog>();
        graceful_stop_dialog_->setLabelText(
            tr("Waiting for the process to end... Please do not close the application."));
        graceful_stop_dialog_->setRange(0, 0);
        graceful_stop_dialog_->setModal(/*modal*/ true);
        graceful_stop_dialog_->setCancelButton(nullptr);

        graceful_stop_dialog_->show();
    }
}

void MainWindow::refresh_ui()
{
    settings_to_ui(settings_, *ui_, module_display_);
    top_bar_bridge_.refresh();
}

void MainWindow::save_settings() noexcept
{
    ui_to_settings(*ui_, top_bar_bridge_, module_display_, settings_);

    if (!cao::save_settings(settings_))
    {
        QMessageBox::critical(this, tr("Error"), tr("Your settings could not be saved to disk."));
    }
}

void MainWindow::run_gui_selector()
{
    auto level_selector = LevelSelector(settings_.gui);
    settings_.gui       = level_selector.run_selection();
    refresh_ui();
}

[[maybe_unused]] void MainWindow::closeEvent(QCloseEvent *event)
{
    stop_process_gracefully();
    save_settings();
    event->accept();
    exit(0);
}

[[maybe_unused]] void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

[[maybe_unused]] void MainWindow::dropEvent(QDropEvent *e)
{
    const QString &file_name = e->mimeData()->urls().at(0).toLocalFile();
    if (std::filesystem::is_directory(file_name.toStdString()))
        ui_->inputDirTextEdit->setText(QDir::cleanPath(file_name));
}

void MainWindow::on_top_bar_popup_open_changed()
{
    sync_top_bar_widget_geometry();
}

void MainWindow::sync_top_bar_widget_geometry()
{
    const bool any_popup_open = top_bar_root_ && top_bar_root_->property("anyPopupOpen").toBool();
    const QRect container_rect = ui_->topBarContainer->geometry();
    top_bar_widget_->setGeometry(container_rect.x(),
                                 container_rect.y(),
                                 container_rect.width(),
                                 any_popup_open ? 300 : 76);
}

void MainWindow::sync_nebula_background_geometry()
{
    // menuBar and centralwidget are both direct children of `this` (QMainWindowLayout positions
    // them as siblings, not parent/child - see nebula_background_widget_'s declaration in
    // MainWindow.hpp), so both their geometries are already in `this`'s coordinate space - no
    // mapToParent/mapTo needed, just the union of the two rects.
    const QRect combined = ui_->menuBar->geometry().united(ui_->centralwidget->geometry());
    nebula_background_widget_->setGeometry(combined);
}

auto MainWindow::eventFilter(QObject *watched, QEvent *event) -> bool
{
    if (watched == ui_->centralwidget && event->type() == QEvent::Resize)
    {
        sync_nebula_background_geometry();
        return false;
    }

    if (watched == ui_->topBarContainer && event->type() == QEvent::Resize)
    {
        sync_top_bar_widget_geometry();
        return false;
    }

    return QMainWindow::eventFilter(watched, event);
}

} // namespace cao
