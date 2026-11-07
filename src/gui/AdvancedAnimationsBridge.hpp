/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>

namespace cao {

/// @brief Thin QML-facing adapter for AdvancedAnimationsModule - a "compress animations" toggle,
/// a "remove redundant keys" (dedupe) toggle and the three error-budget limits (rotation degrees,
/// translation units, scale). Every other btu::kf::Settings field (ratio, compact16, blacklist, etc.) is
/// hardcoded to the known-good values in AdvancedAnimationsModule::ui_to_settings, not exposed
/// here at all - see that file for why. Deliberately just holds UI state - AdvancedAnimationsModule
/// still owns reading it into Settings (ui_to_settings) and pushing Settings into it
/// (settings_to_ui), same split of responsibility as every other module bridge.
class AdvancedAnimationsBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool dedupe READ dedupe WRITE setDedupe NOTIFY dedupeChanged)
    Q_PROPERTY(double maxRotDeg READ maxRotDeg WRITE setMaxRotDeg NOTIFY maxRotDegChanged)
    Q_PROPERTY(double maxTrans READ maxTrans WRITE setMaxTrans NOTIFY maxTransChanged)
    Q_PROPERTY(double maxScale READ maxScale WRITE setMaxScale NOTIFY maxScaleChanged)

public:
    using QObject::QObject;

    [[nodiscard]] auto enabled() const -> bool;
    void setEnabled(bool value);

    [[nodiscard]] auto dedupe() const -> bool;
    void setDedupe(bool value);

    [[nodiscard]] auto maxRotDeg() const -> double;
    void setMaxRotDeg(double value);

    [[nodiscard]] auto maxTrans() const -> double;
    void setMaxTrans(double value);

    [[nodiscard]] auto maxScale() const -> double;
    void setMaxScale(double value);

signals:
    void enabledChanged();
    void dedupeChanged();
    void maxRotDegChanged();
    void maxTransChanged();
    void maxScaleChanged();

private:
    bool enabled_ = true;
    bool dedupe_  = true;
    double max_rot_deg_ = 0.25;
    double max_trans_   = 0.05;
    double max_scale_   = 0.001;
};

} // namespace cao
