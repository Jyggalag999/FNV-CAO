/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "TopBarBridge.hpp"

#include "utils/utils.hpp"

namespace cao {

TopBarBridge::TopBarBridge(const Settings &settings, QObject *parent)
    : QObject(parent)
    , settings_(settings)
{
}

auto TopBarBridge::currentProfile() const -> QString
{
    return to_qstring(settings_.current_profile_name());
}

auto TopBarBridge::profileList() const -> QStringList
{
    // Mirrors settings_to_ui()'s `profiles` lambda: QuickOptimize mode offers a fixed choice of
    // the two base games rather than the user's saved profile list.
    if (settings_.gui.gui_mode == GuiMode::QuickOptimize)
        return {QStringLiteral("SLE"), QStringLiteral("SSE")};

    QStringList result;
    for (const auto &name : settings_.list_profiles())
        result << to_qstring(name);
    return result;
}

auto TopBarBridge::currentPattern() const -> QString
{
    return to_qstring(current_per_file_settings(settings_).pattern.text());
}

auto TopBarBridge::patternList() const -> QStringList
{
    QStringList result;
    for (const auto *per_file_settings : settings_.current_profile().per_file_settings())
        result << to_qstring(per_file_settings->pattern.text());
    return result;
}

auto TopBarBridge::patternsVisible() const -> bool
{
    // Mirrors set_gui_level()'s set_patterns_enabled(ui, /*state=*/true) call, which only ever
    // fires for GuiMode::Advanced.
    return settings_.gui.gui_mode == GuiMode::Advanced;
}

auto TopBarBridge::manageProfilesVisible() const -> bool
{
    // Mirrors set_gui_level()'s ui.manageProfiles->setHidden(level == GuiMode::QuickOptimize).
    return settings_.gui.gui_mode != GuiMode::QuickOptimize;
}

void TopBarBridge::refresh()
{
    emit currentProfileChanged();
    emit profileListChanged();
    emit currentPatternChanged();
    emit patternListChanged();
    emit patternsVisibleChanged();
    emit manageProfilesVisibleChanged();
}

void TopBarBridge::run()
{
    emit runRequested();
}

void TopBarBridge::selectProfile(const QString &name)
{
    emit profileSelected(name);
}

void TopBarBridge::selectPattern(const QString &pattern)
{
    emit patternSelected(pattern);
}

void TopBarBridge::manageProfiles()
{
    emit manageProfilesRequested();
}

void TopBarBridge::managePatterns()
{
    emit managePatternsRequested();
}

} // namespace cao
