/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>

namespace cao {

/// @brief Thin QML-facing adapter for AdvancedAnimationsModule's single "Necessary optimization"
/// checkbox. Deliberately just holds UI state - AdvancedAnimationsModule still owns reading it
/// into Settings (ui_to_settings) and pushing Settings into it (settings_to_ui), same split of
/// responsibility as SelectGpuBridge/TopBarBridge.
class AdvancedAnimationsBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool necessaryOpt READ necessaryOpt WRITE setNecessaryOpt NOTIFY necessaryOptChanged)

public:
    using QObject::QObject;

    [[nodiscard]] auto necessaryOpt() const -> bool;
    void setNecessaryOpt(bool value);

signals:
    void necessaryOptChanged();

private:
    bool necessary_opt_ = false;
};

} // namespace cao
