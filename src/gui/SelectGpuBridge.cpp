/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "SelectGpuBridge.hpp"

#include "utils/utils.hpp"

namespace cao {

SelectGpuBridge::SelectGpuBridge(std::vector<btu::tex::CompressionDevice::AdapterInfo> devices,
                                 QObject *parent)
    : QObject(parent)
    , devices_(std::move(devices))
{
}

auto SelectGpuBridge::deviceNames() const -> QStringList
{
    QStringList result;
    for (const auto &device : devices_)
        result << to_qstring(device.name);
    return result;
}

auto SelectGpuBridge::selectedIndex() const -> int
{
    return selected_index_;
}

void SelectGpuBridge::setSelectedIndex(int index)
{
    if (index < 0 || static_cast<size_t>(index) >= devices_.size())
        return;

    if (selected_index_ == index)
        return;

    selected_index_ = index;
    emit selectedIndexChanged();
}

void SelectGpuBridge::accept()
{
    emit accepted();
}

void SelectGpuBridge::reject()
{
    emit rejected();
}

} // namespace cao
