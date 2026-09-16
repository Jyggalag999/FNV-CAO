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

} // namespace cao
