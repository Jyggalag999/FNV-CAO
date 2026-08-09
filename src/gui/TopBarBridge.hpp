/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/settings.hpp"

#include <QObject>
#include <QString>
#include <QStringList>

namespace cao {

/// @brief Thin QML-facing adapter over Settings, exposing only what MainWindow's top bar
/// (profile selector, pattern selector, Run button) needs today - current profile name, the
/// list of available profiles, current pattern, and a Run trigger. Deliberately minimal per the
/// migration plan: no Settings/Profile mutation lives here, and the full object tree isn't
/// exposed. Profile/pattern *switching* isn't wired up yet either - that lands whenever the real
/// top bar actually gets ported (see the migration plan's step list) and this bridge grows to
/// match, not before.
///
/// Actions stay just as thin: run() doesn't perform the run itself - save_settings(),
/// check_settings(), and Manager orchestration all stay in MainWindow exactly as they are today.
/// It only relays the request via a signal, mirroring the existing
/// processButton::pressed -> MainWindow::init_process connection.
///
/// Non-owning: holds a reference to MainWindow's existing Settings, so it must not outlive it.
/// Settings can change underneath this bridge (ProfilesManagerWindow, PatternsManagerWindow, the
/// profile/pattern combo boxes, ...) - call refresh() after any of that happens, mirroring the
/// existing settings_to_ui() call sites, so QML-bound properties stay in sync.
class TopBarBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentProfile READ currentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(QStringList profileList READ profileList NOTIFY profileListChanged)
    Q_PROPERTY(QString currentPattern READ currentPattern NOTIFY currentPatternChanged)

public:
    explicit TopBarBridge(const Settings &settings, QObject *parent = nullptr);

    [[nodiscard]] auto currentProfile() const -> QString;
    [[nodiscard]] auto profileList() const -> QStringList;
    [[nodiscard]] auto currentPattern() const -> QString;

    /// Re-reads Settings and emits the NOTIFY signals. Call after anything mutates the
    /// underlying Settings out from under this bridge.
    void refresh();

    Q_INVOKABLE void run();

signals:
    void currentProfileChanged();
    void profileListChanged();
    void currentPatternChanged();

    /// QML asked to run; MainWindow owns actually doing it.
    void runRequested();

private:
    const Settings &settings_;
};

} // namespace cao
