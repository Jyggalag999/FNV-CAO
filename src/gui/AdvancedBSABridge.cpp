/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "AdvancedBSABridge.hpp"

namespace cao {

auto AdvancedBSABridge::packFile() const -> bool
{
    return pack_file_;
}

void AdvancedBSABridge::setPackFile(bool value)
{
    if (pack_file_ == value)
        return;

    pack_file_ = value;
    emit packFileChanged();
}

} // namespace cao
