/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "AdvancedMeshesBridge.hpp"

namespace cao {

auto AdvancedMeshesBridge::baseChecked() const -> bool
{
    return base_checked_;
}

void AdvancedMeshesBridge::setBaseChecked(bool value)
{
    if (base_checked_ == value)
        return;

    base_checked_ = value;
    emit baseCheckedChanged();

    if (value && full_optimization_)
    {
        full_optimization_ = false;
        emit fullOptimizationChanged();
    }
}

auto AdvancedMeshesBridge::fullOptimization() const -> bool
{
    return full_optimization_;
}

void AdvancedMeshesBridge::setFullOptimization(bool value)
{
    if (full_optimization_ == value)
        return;

    full_optimization_ = value;
    emit fullOptimizationChanged();
}

} // namespace cao
