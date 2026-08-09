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
    QStringList result;
    for (const auto &name : settings_.list_profiles())
        result << to_qstring(name);
    return result;
}

auto TopBarBridge::currentPattern() const -> QString
{
    return to_qstring(current_per_file_settings(settings_).pattern.text());
}

void TopBarBridge::refresh()
{
    emit currentProfileChanged();
    emit profileListChanged();
    emit currentPatternChanged();
}

void TopBarBridge::run()
{
    emit runRequested();
}

} // namespace cao
