/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <btu/tex/compression_device.hpp>

#include <QObject>
#include <QString>
#include <QStringList>

#include <vector>

namespace cao {

/// @brief Thin QML-facing adapter for SelectGPUWindow: the list of available GPU adapters, which
/// one is selected, and accept/reject actions. Owns the device list (queried once at
/// construction, same as the Widgets version did) - SelectGPUWindow just hosts this and the QML
/// view over it, and relays accepted()/rejected() to QDialog::accept()/reject().
class SelectGpuBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList deviceNames READ deviceNames CONSTANT)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)

public:
    explicit SelectGpuBridge(std::vector<btu::tex::CompressionDevice::AdapterInfo> devices,
                             QObject *parent = nullptr);

    [[nodiscard]] auto deviceNames() const -> QStringList;

    [[nodiscard]] auto selectedIndex() const -> int;
    void setSelectedIndex(int index);

    Q_INVOKABLE void accept();
    Q_INVOKABLE void reject();

signals:
    void selectedIndexChanged();
    void accepted();
    void rejected();

private:
    std::vector<btu::tex::CompressionDevice::AdapterInfo> devices_;
    int selected_index_ = -1;
};

} // namespace cao
