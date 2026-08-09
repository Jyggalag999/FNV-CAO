/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>

namespace cao {

/// @brief Thin QML-facing adapter for AdvancedBSAModule's single "Pack file" checkbox. Same
/// pattern as AdvancedAnimationsBridge: just UI state, no signals - AdvancedBSAModule still owns
/// reading/writing Settings around it.
class AdvancedBSABridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool packFile READ packFile WRITE setPackFile NOTIFY packFileChanged)

public:
    using QObject::QObject;

    [[nodiscard]] auto packFile() const -> bool;
    void setPackFile(bool value);

signals:
    void packFileChanged();

private:
    bool pack_file_ = false;
};

} // namespace cao
