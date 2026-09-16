/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>

namespace cao {

/// @brief Thin QML-facing adapter for AdvancedAnimationsModule - a single "compress animations"
/// toggle. Every other btu::kf::Settings field (ratio, compact16, blacklist, etc.) is hardcoded to
/// the known-good/vanilla-matching values in AdvancedAnimationsModule::ui_to_settings, not exposed
/// here at all - see that file for why. Deliberately just holds UI state - AdvancedAnimationsModule
/// still owns reading it into Settings (ui_to_settings) and pushing Settings into it
/// (settings_to_ui), same split of responsibility as every other module bridge.
class AdvancedAnimationsBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    using QObject::QObject;

    [[nodiscard]] auto enabled() const -> bool;
    void setEnabled(bool value);

signals:
    void enabledChanged();

private:
    bool enabled_ = true;
};

} // namespace cao
