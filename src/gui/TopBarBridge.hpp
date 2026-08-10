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

/// @brief Thin QML-facing adapter over Settings, exposing what MainWindow's top bar (profile
/// selector, pattern selector, Manage buttons, Run button) needs - current profile name, the
/// list of available profiles, current pattern and its sibling list, gui-mode-dependent
/// visibility for the pattern row and the Manage Profiles button, and triggers for
/// selecting/managing/running. Deliberately minimal per the migration plan: no Settings/Profile
/// mutation lives here, and the full object tree isn't exposed - select*()/manage*()/run() all
/// just relay the request via a signal; MainWindow does the actual work (set_current_profile(),
/// save_settings(), opening ProfilesManagerWindow/PatternsManagerWindow, Manager orchestration),
/// exactly mirroring what the old native-widget connect() lambdas already did.
///
/// Non-owning: holds a reference to MainWindow's existing Settings, so it must not outlive it.
/// Settings can change underneath this bridge (ProfilesManagerWindow, PatternsManagerWindow,
/// profile/pattern selection, gui_mode changes, ...) - call refresh() after any of that happens,
/// mirroring the existing settings_to_ui() call sites, so QML-bound properties stay in sync.
class TopBarBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentProfile READ currentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(QStringList profileList READ profileList NOTIFY profileListChanged)
    Q_PROPERTY(QString currentPattern READ currentPattern NOTIFY currentPatternChanged)
    Q_PROPERTY(QStringList patternList READ patternList NOTIFY patternListChanged)
    Q_PROPERTY(bool patternsVisible READ patternsVisible NOTIFY patternsVisibleChanged)
    Q_PROPERTY(bool manageProfilesVisible READ manageProfilesVisible NOTIFY manageProfilesVisibleChanged)

public:
    explicit TopBarBridge(const Settings &settings, QObject *parent = nullptr);

    [[nodiscard]] auto currentProfile() const -> QString;
    [[nodiscard]] auto profileList() const -> QStringList;
    [[nodiscard]] auto currentPattern() const -> QString;
    [[nodiscard]] auto patternList() const -> QStringList;
    [[nodiscard]] auto patternsVisible() const -> bool;
    [[nodiscard]] auto manageProfilesVisible() const -> bool;

    /// Re-reads Settings and emits the NOTIFY signals. Call after anything mutates the
    /// underlying Settings out from under this bridge.
    void refresh();

    Q_INVOKABLE void run();
    Q_INVOKABLE void selectProfile(const QString &name);
    Q_INVOKABLE void selectPattern(const QString &pattern);
    Q_INVOKABLE void manageProfiles();
    Q_INVOKABLE void managePatterns();

signals:
    void currentProfileChanged();
    void profileListChanged();
    void currentPatternChanged();
    void patternListChanged();
    void patternsVisibleChanged();
    void manageProfilesVisibleChanged();

    /// QML asked to run/select/manage; MainWindow owns actually doing it.
    void runRequested();
    void profileSelected(QString name);
    void patternSelected(QString pattern);
    void manageProfilesRequested();
    void managePatternsRequested();

private:
    const Settings &settings_;
};

} // namespace cao
