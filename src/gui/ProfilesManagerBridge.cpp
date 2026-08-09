/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "ProfilesManagerBridge.hpp"

namespace cao {

ProfilesManagerBridge::ProfilesManagerBridge(QStringList game_names, QObject *parent)
    : QObject(parent)
    , game_names_(std::move(game_names))
{
}

auto ProfilesManagerBridge::profileNames() const -> QStringList
{
    return profile_names_;
}

void ProfilesManagerBridge::setProfileNames(QStringList names)
{
    if (profile_names_ == names)
        return;

    profile_names_ = std::move(names);
    emit profileNamesChanged();
}

auto ProfilesManagerBridge::currentProfile() const -> QString
{
    return current_profile_;
}

void ProfilesManagerBridge::setCurrentProfile(const QString &name)
{
    if (current_profile_ == name)
        return;

    current_profile_ = name;
    emit currentProfileChanged();
}

auto ProfilesManagerBridge::gameNames() const -> QStringList
{
    return game_names_;
}

auto ProfilesManagerBridge::currentGameIndex() const -> int
{
    return current_game_index_;
}

void ProfilesManagerBridge::setCurrentGameIndex(int index)
{
    if (current_game_index_ == index)
        return;

    current_game_index_ = index;
    emit currentGameIndexChanged();
}

void ProfilesManagerBridge::requestSelectProfile(const QString &name)
{
    emit selectProfileRequested(name);
}

void ProfilesManagerBridge::requestSelectGame(int index)
{
    if (index < 0 || index >= game_names_.size())
        return;

    emit selectGameRequested(index);
}

void ProfilesManagerBridge::requestNewProfile()
{
    emit newProfileRequested();
}

void ProfilesManagerBridge::requestRemoveProfile()
{
    emit removeProfileRequested();
}

void ProfilesManagerBridge::requestImportProfile()
{
    emit importProfileRequested();
}

void ProfilesManagerBridge::requestExportProfile()
{
    emit exportProfileRequested();
}

} // namespace cao
