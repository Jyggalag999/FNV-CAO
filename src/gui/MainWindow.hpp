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

class QQuickWidget;

namespace Ui {
class MainWindow;
} // namespace Ui

namespace cao {
class Manager;
class Settings;

class MainWindow final : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

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
    TopBarBridge top_bar_bridge_;

    // Step 1/2 QQuickWidget scaffold, currently proving TopBarBridge round-trips real Settings
    // data (see src/gui/qml/TopBarBridgeDemo.qml). Parented to centralwidget, so Qt owns/destroys
    // it normally - no manual cleanup needed. Remove once the real top bar is ported (see plan).
    QQuickWidget *qml_poc_widget_ = nullptr;

    void init_process();
    void stop_process_gracefully();

    void save_settings() noexcept;

    // Refreshes the Widgets UI from settings_, same as settings_to_ui() always did, and keeps
    // top_bar_bridge_'s QML-facing properties in sync with it.
    void refresh_ui();

    void run_gui_selector();

    void about() noexcept;

    // Qt override
    [[maybe_unused]] void closeEvent(QCloseEvent *event) override;
    [[maybe_unused]] void dragEnterEvent(QDragEnterEvent *e) override;
    [[maybe_unused]] void dropEvent(QDropEvent *e) override;
    auto eventFilter(QObject *watched, QEvent *event) -> bool override;
};
} // namespace cao
