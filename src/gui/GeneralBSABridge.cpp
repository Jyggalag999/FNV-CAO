/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "GeneralBSABridge.hpp"

namespace cao {

auto GeneralBSABridge::baseChecked() const -> bool
{
    return base_checked_;
}

void GeneralBSABridge::setBaseChecked(bool value)
{
    if (base_checked_ == value)
        return;

    base_checked_ = value;
    emit baseCheckedChanged();

    if (value && extract_mode_)
    {
        extract_mode_ = false;
        emit extractModeChanged();
    }
}

auto GeneralBSABridge::extractMode() const -> bool
{
    return extract_mode_;
}

void GeneralBSABridge::setExtractMode(bool value)
{
    if (extract_mode_ == value)
        return;

    extract_mode_ = value;
    emit extractModeChanged();
}

auto GeneralBSABridge::dontMakeLoaded() const -> bool
{
    return dont_make_loaded_;
}

void GeneralBSABridge::setDontMakeLoaded(bool value)
{
    if (dont_make_loaded_ == value)
        return;

    dont_make_loaded_ = value;
    emit dontMakeLoadedChanged();
}

auto GeneralBSABridge::dontRemoveFiles() const -> bool
{
    return dont_remove_files_;
}

void GeneralBSABridge::setDontRemoveFiles(bool value)
{
    if (dont_remove_files_ == value)
        return;

    dont_remove_files_ = value;
    emit dontRemoveFilesChanged();
}

auto GeneralBSABridge::dontCompress() const -> bool
{
    return dont_compress_;
}

void GeneralBSABridge::setDontCompress(bool value)
{
    if (dont_compress_ == value)
        return;

    dont_compress_ = value;
    emit dontCompressChanged();
}

auto GeneralBSABridge::makeOverrides() const -> bool
{
    return make_overrides_;
}

void GeneralBSABridge::setMakeOverrides(bool value)
{
    if (make_overrides_ == value)
        return;

    make_overrides_ = value;
    emit makeOverridesChanged();
}

auto GeneralBSABridge::makeOverridesVisible() const -> bool
{
    return make_overrides_visible_;
}

void GeneralBSABridge::setMakeOverridesVisible(bool value)
{
    if (make_overrides_visible_ == value)
        return;

    make_overrides_visible_ = value;
    emit makeOverridesVisibleChanged();
}

auto GeneralBSABridge::archiveName() const -> QString
{
    return archive_name_;
}

void GeneralBSABridge::setArchiveName(const QString &value)
{
    if (archive_name_ == value)
        return;

    archive_name_ = value;
    emit archiveNameChanged();
}

auto GeneralBSABridge::overrideMaxSize() const -> bool
{
    return override_max_size_;
}

void GeneralBSABridge::setOverrideMaxSize(bool value)
{
    if (override_max_size_ == value)
        return;

    override_max_size_ = value;
    emit overrideMaxSizeChanged();
}

auto GeneralBSABridge::maxSizeMb() const -> int
{
    return max_size_mb_;
}

void GeneralBSABridge::setMaxSizeMb(int value)
{
    if (max_size_mb_ == value)
        return;

    max_size_mb_ = value;
    emit maxSizeMbChanged();
}

auto GeneralBSABridge::defaultMaxSizeMb() const -> int
{
    return default_max_size_mb_;
}

void GeneralBSABridge::setDefaultMaxSizeMb(int value)
{
    if (default_max_size_mb_ == value)
        return;

    default_max_size_mb_ = value;
    emit defaultMaxSizeMbChanged();
}

} // namespace cao
