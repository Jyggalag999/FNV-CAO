/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "IWindowModule.hpp"
#include "ModuleDisplay.hpp"
#include "ProgressWindow.hpp"
#include "TopBarBridge.hpp"
#include "settings/settings.hpp"

#include <QCoreApplication>
#include <QMainWindow>
#include <QProgressDialog>
#include <future>

class QQuickItem;
class QQuickWidget;

namespace Ui {
class MainWindow;
} // namespace Ui

namespace cao {
class Manager;
class Settings;

class MainWindow final : public QMainWindow
{
    // Needed for on_top_bar_popup_open_changed() below: it's reached via a string-based SLOT()
    // connection (see the constructor), which requires MOC to have generated a meta-call entry
    // for it. Every other connection in this class is modern pointer-to-member/functor style,
    // which doesn't need Q_OBJECT on the receiver - this is the first one that does. Supersedes
    // (and conflicts with, if both are present) the old Q_DECLARE_TR_FUNCTIONS(MainWindow) - that
    // macro is for non-QObject classes that still want scoped tr(); Q_OBJECT already provides one
    // scoped to "MainWindow" via this class's own metaObject.
    Q_OBJECT

public:
    explicit MainWindow(Settings settings, QWidget *parent = nullptr);

    MainWindow(const MainWindow &) = delete;
    MainWindow(MainWindow &&)      = delete;

    auto operator=(const MainWindow &) -> MainWindow & = delete;
    auto operator=(MainWindow &&) -> MainWindow      & = delete;

    ~MainWindow() override;

private:
    Settings settings_;

    std::unique_ptr<Ui::MainWindow> ui_;
    std::optional<std::jthread> cao_process_;
    std::unique_ptr<QProgressDialog> graceful_stop_dialog_;

    std::unique_ptr<ProgressWindow> progress_window_;
    ModuleDisplay module_display_{};

    // Step 2 bridge: exposes what the (still-Widgets) top bar needs to QML. Declared after
    // settings_ deliberately - it holds a reference to it, so settings_ must construct first.
    // The Step 1/2 QQuickWidget scaffold that proved this against real Settings data (see git
    // history for TopBarBridgeDemo.qml) has been removed now that it's served its purpose - it
    // was a fixed corner patch deliberately covering part of the real Widgets UI, not real UI
    // itself. top_bar_bridge_ stays: it's reused as-is when the real top bar is ported (see plan).
    TopBarBridge top_bar_bridge_;

    // Step 5: QML replacement for the old QPainter/eventFilter nebula background (see
    // src/gui/qml/NebulaBackground.qml). Parented to `this` (the QMainWindow), not centralwidget -
    // menuBar and centralwidget are siblings under QMainWindow's own layout, not parent/child, so
    // a background meant to show behind *both* (menuBar's "Options" reads as plain floating text
    // now - see MainWindow.cpp's nebula_overrides QMenuBar rules - with nothing behind it unless
    // this extends up into that row too) has to be parented above both of them, not inside just
    // one. Kept lowered to the back of its stacking order (see the constructor) so the real
    // layout-managed widgets - menuBar, groupBox, inputDirTextEdit, mainGroupBox, tabWidget - all
    // paint on top of it. Geometry is the union of menuBar's and centralwidget's own geometry
    // (both already in `this`'s coordinate space, being its direct children) - kept in sync via
    // eventFilter() reacting to centralwidget's QEvent::Resize (see sync_nebula_background_geometry()
    // - menuBar's height is stable, so watching centralwidget alone is enough to know when the
    // union needs recomputing), since this widget isn't itself layout-managed.
    QQuickWidget *nebula_background_widget_ = nullptr;

    // Step 5: real top bar (see src/gui/qml/TopBar.qml). Parented directly to centralwidget and
    // manually positioned/sized (see sync_top_bar_widget_geometry()) rather than added to
    // topBarContainer's layout, same reasoning as nebula_background_widget_ below:
    // on_top_bar_popup_open_changed() grows this widget while a dropdown is open so its popup
    // isn't clipped (a QQuickWidget can't render content past its own bounds, unlike a native
    // QComboBox's popup, which is its own top-level window) - growing a *layout-managed* widget
    // necessarily pushes every sibling below it down, which is exactly the bug this avoids.
    // topBarContainer stays in the layout as a fixed-height (76px) placeholder purely to reserve
    // the right amount of space for the closed state; the real widget floats above it (raised, in
    // centralwidget's stacking order) and overlaps mainGroupBox/tabWidget while a popup is open,
    // instead of pushing them down. Kept as members (rather than constructor-locals) because
    // on_top_bar_popup_open_changed() needs to reach both: TopBar.qml's root aggregates its combo
    // boxes' popup-open state into a plain QML property (anyPopupOpen), not a compile-time
    // Q_PROPERTY, so connecting to its change notification from C++ needs the old string-based
    // SIGNAL()/SLOT() syntax - which requires an actual slot method, not a lambda.
    QQuickWidget *top_bar_widget_ = nullptr;
    QQuickItem *top_bar_root_     = nullptr;

    void init_process();
    void stop_process_gracefully();

    void save_settings() noexcept;

    // Refreshes the Widgets UI from settings_, same as settings_to_ui() always did, and keeps
    // top_bar_bridge_'s QML-facing properties in sync with it.
    void refresh_ui();

    void run_gui_selector();

    // A QQuickWidget can't render content past its own bounds (unlike a native QComboBox's
    // popup, which is its own top-level window) - grows top_bar_widget_ while a dropdown is open
    // so it isn't clipped, then shrinks it back, instead of permanently reserving that space.
    Q_SLOT void on_top_bar_popup_open_changed();

    // Repositions/resizes top_bar_widget_ to match topBarContainer's current placement (x/y/
    // width - called from eventFilter() on topBarContainer's QEvent::Resize) while independently
    // choosing its own height based on whether a dropdown is currently open (76px closed, 300px
    // while any popup needs room) - called from both eventFilter() and
    // on_top_bar_popup_open_changed(), since either can happen without the other.
    void sync_top_bar_widget_geometry();

    // Repositions/resizes nebula_background_widget_ to cover menuBar *and* centralwidget - called
    // from eventFilter() on centralwidget's QEvent::Resize (menuBar's own height is stable, so
    // tracking centralwidget resize alone is enough to know when the union needs recomputing).
    // See nebula_background_widget_'s own declaration for why it spans both instead of just
    // centralwidget.
    void sync_nebula_background_geometry();

    // Qt override
    [[maybe_unused]] void closeEvent(QCloseEvent *event) override;
    [[maybe_unused]] void dragEnterEvent(QDragEnterEvent *e) override;
    [[maybe_unused]] void dropEvent(QDropEvent *e) override;
    auto eventFilter(QObject *watched, QEvent *event) -> bool override;
};
} // namespace cao
