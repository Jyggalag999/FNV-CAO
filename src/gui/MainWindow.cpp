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
#include <QUrl>

namespace cao {
constexpr static inline auto k_discord_url = "https://discord.gg/SwfTzHGQcy";
constexpr static inline auto k_nexus_url   = "https://www.nexusmods.com/skyrimspecialedition/mods/23316";
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
    base.replace(QStringLiteral("#32414B"), QStringLiteral("#4a2c6d"), Qt::CaseInsensitive); // borders (most uses), some fills
    base.replace(QStringLiteral("#19232D"), QStringLiteral("#170c26"), Qt::CaseInsensitive); // dark fills
    base.replace(QStringLiteral("#F0F0F0"), QStringLiteral("#e6d8ef"), Qt::CaseInsensitive); // light/white text
    base.replace(QStringLiteral("#148CD2"), QStringLiteral("#d98fe0"), Qt::CaseInsensitive); // blue hover/selection accent
    base.replace(QStringLiteral("#787878"), QStringLiteral("#6a5a76"), Qt::CaseInsensitive); // disabled gray text/fills
    base.replace(QStringLiteral("#1464A0"), QStringLiteral("#642878"), Qt::CaseInsensitive); // blue selected accent
    base.replace(QStringLiteral("#14506E"), QStringLiteral("#2a1c3a"), Qt::CaseInsensitive); // dim blue (disabled+selected)
    base.replace(QStringLiteral("#505F69"), QStringLiteral("#50145a"), Qt::CaseInsensitive); // lighter gray-blue fill

    // Nebula palette sampled from a real nebula photo: deep magenta body, azure-blue wisps,
    // burnt-orange/gold rim light, lavender-white star highlights. Layered after the qdarkstyle
    // base so its widget-chrome icons/arrows keep working; later rules win on equal specificity.
    static const QString nebula_overrides = R"(
        QWidget {
            color: #e6d8ef;
        }
        QWidget:disabled {
            background-color: transparent;
            color: #6a5a76;
        }
        #centralwidget {
            background-color: transparent;
        }
        QMainWindow {
            background-color: transparent;
        }
        QDialog {
            background-color: #0a0512;
        }
        QGroupBox {
            background-color: #170c26;
            color: #f0d2e1;
        }
        QGroupBox::title {
            color: #d98fe0;
        }
        QGroupBox::indicator {
            width: 12px;
            height: 12px;
        }
        QPushButton {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                               stop:0 #3c1450, stop:1 #280028);
            border: 1px solid #4a2c6d;
            color: #f0e0f5;
        }
        QPushButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                               stop:0 #50145a, stop:1 #3c0032);
            border: 1px solid #642878;
        }
        QPushButton:pressed {
            background-color: #962d00;
            border: 1px solid #4a2c6d;
        }
        QPushButton:disabled {
            background-color: #201530;
            border: 1px solid #2a1c3a;
            color: #6a5a76;
        }
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #170c26;
            border: 1px solid #4a2c6d;
            border-radius: 3px;
            color: #e6d8ef;
            selection-background-color: #642878;
        }
        /* Base qdarkstyle leaves QSpinBox/QComboBox borders at its own default gray-blue
           (#32414B) since this block only overrode fill/text/selection colors - the border was
           never brought into the nebula palette. Same background/border fix applied to
           QComboBox's popup list below. */
        QSpinBox, QComboBox {
            background-color: #170c26;
            border: 1px solid #4a2c6d;
            color: #e6d8ef;
            selection-background-color: #642878;
        }
        QSpinBox:disabled, QComboBox:disabled {
            background-color: #201530;
            border: 1px solid #2a1c3a;
            color: #6a5a76;
        }
        QComboBox::drop-down {
            border-left: 1px solid #4a2c6d;
        }
        QComboBox QAbstractItemView {
            background-color: #170c26;
            border: 1px solid #642878;
            color: #e6d8ef;
            selection-background-color: #642878;
            selection-color: #ffffff;
        }
        /* Focus ring was blue (#2d8ae0) - the one interactive state nebula_overrides hadn't
           actually themed purple yet, so tabbing into (or clicking) a field showed a jarring
           native-looking blue outline against everything else's purple. */
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
            border: 1px solid #d98fe0;
        }
        QCheckBox, QRadioButton {
            background-color: transparent;
            color: #e6d8ef;
        }
        QRadioButton:disabled {
            background-color: transparent;
            color: #6a5a76;
        }
        /* QCheckBox/QRadioButton/QMenu's checkable-item indicators are qdarkstyle bitmap icons
           (checkbox_checked.png etc.) with the gray-blue palette baked into the pixels - QSS text
           substitution can't recolor an image. Overriding the indicator subcontrols with plain
           background-color/border (no image) replaces those icons with a flat swatch instead,
           matching the app's own hand-rolled CaoCheckBox.qml (dark fill, purple border, filled
           lavender square when checked). Base styles these through two-pseudo-state selectors
           (QCheckBox::indicator:unchecked:hover, etc.), so matching selectors are needed here too
           - a single-pseudo-state QCheckBox::indicator:hover would lose to them, same as the
           QTabBar::tab:top/QScrollBar::handle:vertical specificity issue elsewhere in this sheet.
           `image` is also a separate property from `background-color`/`border` - the base icon
           still drew on *top* of a plain background-color override otherwise (as a literal blue
           checkmark glyph sitting over a correctly-purple square), so every rule below explicitly
           clears it. */
        QCheckBox::indicator:unchecked, QCheckBox::indicator:indeterminate {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #170c26;
            border: 1px solid #4a2c6d;
            border-radius: 2px;
        }
        QCheckBox::indicator:unchecked:hover, QCheckBox::indicator:unchecked:focus,
        QCheckBox::indicator:unchecked:pressed,
        QCheckBox::indicator:indeterminate:hover, QCheckBox::indicator:indeterminate:focus,
        QCheckBox::indicator:indeterminate:pressed {
            image: none;
            border: 1px solid #642878;
        }
        QCheckBox::indicator:unchecked:disabled, QCheckBox::indicator:indeterminate:disabled {
            image: none;
            background-color: #201530;
            border: 1px solid #2a1c3a;
        }
        QCheckBox::indicator:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #d98fe0;
            border: 1px solid #d98fe0;
            border-radius: 2px;
        }
        QCheckBox::indicator:checked:hover, QCheckBox::indicator:checked:focus,
        QCheckBox::indicator:checked:pressed {
            image: none;
            background-color: #f0d2e1;
            border: 1px solid #f0d2e1;
        }
        QCheckBox::indicator:checked:disabled {
            image: none;
            background-color: #6a5a76;
            border: 1px solid #2a1c3a;
        }
        QRadioButton::indicator:unchecked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #170c26;
            border: 1px solid #4a2c6d;
            border-radius: 6px;
        }
        QRadioButton::indicator:unchecked:hover, QRadioButton::indicator:unchecked:focus,
        QRadioButton::indicator:unchecked:pressed {
            image: none;
            border: 1px solid #642878;
        }
        QRadioButton::indicator:unchecked:disabled {
            image: none;
            background-color: #201530;
            border: 1px solid #2a1c3a;
        }
        QRadioButton::indicator:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #170c26;
            border: 3px solid #d98fe0;
            border-radius: 6px;
        }
        QRadioButton::indicator:checked:hover, QRadioButton::indicator:checked:focus,
        QRadioButton::indicator:checked:pressed {
            image: none;
            border: 3px solid #f0d2e1;
        }
        QRadioButton::indicator:checked:disabled {
            image: none;
            background-color: #201530;
            border: 3px solid #2a1c3a;
        }
        QMenu::indicator:non-exclusive:unchecked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #170c26;
            border: 1px solid #4a2c6d;
            border-radius: 2px;
        }
        QMenu::indicator:non-exclusive:unchecked:selected {
            image: none;
            border: 1px solid #642878;
        }
        QMenu::indicator:non-exclusive:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #d98fe0;
            border: 1px solid #d98fe0;
            border-radius: 2px;
        }
        QMenu::indicator:non-exclusive:checked:selected {
            image: none;
            background-color: #f0d2e1;
            border: 1px solid #f0d2e1;
        }
        QMenu::indicator:exclusive:unchecked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #170c26;
            border: 1px solid #4a2c6d;
            border-radius: 6px;
        }
        QMenu::indicator:exclusive:unchecked:selected {
            image: none;
            border: 1px solid #642878;
        }
        QMenu::indicator:exclusive:checked {
            image: none;
            width: 12px;
            height: 12px;
            background-color: #170c26;
            border: 3px solid #d98fe0;
            border-radius: 6px;
        }
        QMenu::indicator:exclusive:checked:selected {
            image: none;
            border: 3px solid #f0d2e1;
        }
        QTabWidget::pane {
            border: 1px solid #4a2c6d;
            background-color: #0d0818;
        }
        QTabWidget::pane:selected {
            background-color: #170c26;
            border: 1px solid #642878;
        }
        /* Base qdarkstyle only really styles tabs through its :top/:bottom/:left/:right variants
           (e.g. QTabBar::tab:top, QTabBar::tab:top:selected) - those are more specific than a
           plain QTabBar::tab/:selected/:hover (extra pseudo-state), so they always won regardless
           of cascade order, leaving tabs on qdarkstyle's own gray-blue (#32414B/#505F69) fill with
           a blue (#1464A0/#148CD2) selected/hover accent. Matching the same :top selectors here
           (tabs only ever appear on top in this app) recolors them without touching the
           shape/spacing properties (padding, margins, corner radius) those base rules also set,
           which are left alone. */
        QTabBar::tab:top {
            background-color: #170c26;
            color: #c4a8d4;
            border: 1px solid #4a2c6d;
        }
        QTabBar::tab:top:selected {
            background-color: #3c1450;
            color: #f0d2e1;
            border: 1px solid #642878;
        }
        QTabBar::tab:top:!selected:hover {
            background-color: #241238;
            color: #ffffff;
            border: 1px solid #642878;
        }
        QListWidget, QTreeWidget, QTableWidget {
            background-color: #120a1e;
            border: 1px solid #4a2c6d;
            alternate-background-color: #170c26;
        }
        QListWidget::item:selected, QTreeWidget::item:selected, QTableWidget::item:selected {
            background-color: #642878;
            color: #ffffff;
        }
        QListWidget::item:hover, QTreeWidget::item:hover {
            background-color: #3c1450;
        }
        QProgressBar {
            border: 1px solid #642878;
            border-radius: 3px;
            background-color: #170c26;
            text-align: center;
            color: #f0d2e1;
        }
        QProgressBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                               stop:0 #2d5aa5, stop:0.5 #964fb0, stop:1 #d97a2d);
            border-radius: 2px;
        }
        QMenuBar {
            background-color: #0a0512;
            color: #e6d8ef;
        }
        QMenuBar::item:selected {
            background-color: #3c1450;
        }
        QMenu {
            background-color: #170c26;
            border: 1px solid #642878;
            color: #e6d8ef;
        }
        QMenu::item:selected {
            background-color: #642878;
            color: #ffffff;
        }
        QScrollBar:vertical, QScrollBar:horizontal {
            background-color: #0d0818;
        }
        QScrollBar::handle {
            background-color: #4a2c6d;
            border-radius: 3px;
        }
        QScrollBar::handle:hover {
            background-color: #642878;
        }
        QToolTip {
            background-color: #170c26;
            border: 1px solid #d97a2d;
            color: #f0d2e1;
        }
        QLabel {
            color: #e6d8ef;
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
    // eventFilter()'s QEvent::Resize handling below.
    ui_->centralwidget->installEventFilter(this);

    // Step 5: nebula background, parented directly to centralwidget (not added to its layout) so
    // it can sit behind the real layout-managed widgets rather than taking its own row. lower()
    // pushes it to the back of the stacking order now, right after construction, since it would
    // otherwise be topmost (last child added wins the top of the stack in Qt).
    nebula_background_widget_ = new QQuickWidget(ui_->centralwidget);
    nebula_background_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    nebula_background_widget_->setSource(QUrl("qrc:/qml/NebulaBackground.qml"));
    nebula_background_widget_->setGeometry(ui_->centralwidget->rect());
    nebula_background_widget_->lower();

    // Step 5: real top bar, replacing the old native profiles/patterns QGroupBox - added into
    // topBarContainer's (empty, zero-margin) layout the same way every module embeds its QML,
    // unlike nebula_background_widget_ above which deliberately isn't layout-managed.
    top_bar_widget_ = new QQuickWidget(ui_->topBarContainer);
    top_bar_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // Without this, the layout has no way to know how tall to make an otherwise-empty container
    // whose only content is a QQuickWidget using SizeRootObjectToView (its root's size depends on
    // the view's size, which the layout can't determine without a hint from the root) - it
    // collapsed topBarContainer to zero height. 76px covers both rows (2 * 26px content + 8px
    // row spacing + 8px top/bottom margins) - see TopBar.qml.
    top_bar_widget_->setMinimumHeight(76);
    top_bar_widget_->rootContext()->setContextProperty("topBar", &top_bar_bridge_);
    top_bar_widget_->setSource(QUrl("qrc:/qml/TopBar.qml"));
    ui_->topBarContainer->layout()->addWidget(top_bar_widget_);

    top_bar_root_ = top_bar_widget_->rootObject();
    if (top_bar_root_)
        connect(top_bar_root_, SIGNAL(anyPopupOpenChanged()), this, SLOT(on_top_bar_popup_open_changed()));

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

    auto url_opener = [](const char *url) { return [url] { QDesktopServices::openUrl(QUrl(url)); }; };

    connect(ui_->actionDocumentation, &QAction::triggered, this, url_opener(k_nexus_url));
    connect(ui_->actionDiscord, &QAction::triggered, this, url_opener(k_discord_url));
    connect(ui_->actionDonate, &QAction::triggered, this, url_opener(k_kofi_url));

    connect(ui_->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui_->actionAbout_Qt, &QAction::triggered, this, &QApplication::aboutQt);

    // first_start(settings_.gui.first_run); // welcome popup disabled

    // The Level Selector dialog no longer runs at startup - always launch straight into Advanced
    // Mode instead. Still reachable afterward via the "Change level" menu action
    // (actionChange_level, connected to run_gui_selector() above) for anyone who wants a
    // different mode.
    settings_.gui.gui_mode = GuiMode::Advanced;
    refresh_ui();
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

void MainWindow::about() noexcept
{
    constexpr auto message =
        R"( %1 version %2 built on %3
            Made by G'k
            This program is distributed in the hope that it will be useful but WITHOUT ANY WARRANTY.
            See the Mozilla Public License)";

    const QString text = tr(message).arg(QCoreApplication::applicationName(),
                                         QCoreApplication::applicationVersion(),
                                         __DATE__);

    QMessageBox::about(this, tr("About"), text);
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
    const bool any_popup_open = top_bar_root_->property("anyPopupOpen").toBool();
    top_bar_widget_->setFixedHeight(any_popup_open ? 300 : 76);
}

auto MainWindow::eventFilter(QObject *watched, QEvent *event) -> bool
{
    if (watched == ui_->centralwidget && event->type() == QEvent::Resize)
    {
        auto *central = qobject_cast<QWidget *>(watched);
        nebula_background_widget_->setGeometry(central->rect());
        return false;
    }

    return QMainWindow::eventFilter(watched, event);
}

} // namespace cao
