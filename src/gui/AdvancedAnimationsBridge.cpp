/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "AdvancedAnimationsBridge.hpp"

namespace cao {

auto AdvancedAnimationsBridge::enabled() const -> bool
{
    return enabled_;
}

void AdvancedAnimationsBridge::setEnabled(bool value)
{
    if (enabled_ == value)
        return;

    enabled_ = value;
    emit enabledChanged();
}

auto AdvancedAnimationsBridge::dedupe() const -> bool
{
    return dedupe_;
}

void AdvancedAnimationsBridge::setDedupe(bool value)
{
    if (dedupe_ == value)
        return;

    dedupe_ = value;
    emit dedupeChanged();
}

auto AdvancedAnimationsBridge::maxRotDeg() const -> double
{
    return max_rot_deg_;
}

void AdvancedAnimationsBridge::setMaxRotDeg(double value)
{
    if (max_rot_deg_ == value)
        return;

    max_rot_deg_ = value;
    emit maxRotDegChanged();
}

auto AdvancedAnimationsBridge::maxTrans() const -> double
{
    return max_trans_;
}

void AdvancedAnimationsBridge::setMaxTrans(double value)
{
    if (max_trans_ == value)
        return;

    max_trans_ = value;
    emit maxTransChanged();
}

auto AdvancedAnimationsBridge::maxScale() const -> double
{
    return max_scale_;
}

void AdvancedAnimationsBridge::setMaxScale(double value)
{
    if (max_scale_ == value)
        return;

    max_scale_ = value;
    emit maxScaleChanged();
}

} // namespace cao
