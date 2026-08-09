/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>

namespace cao {

/// @brief Thin QML-facing adapter for AdvancedMeshesModule's checkable "Process meshes" group and
/// its two mutually-exclusive radio options (Recommended/Extensive). fullOptimization: false =
/// Recommended selected, true = Extensive selected - a plain bool is enough since exactly one of
/// the two is always the selection whenever baseChecked is true.
///
/// Replicates one behavioral detail of the old Widgets version: turning baseChecked on (false ->
/// true) resets the selection to Recommended, same as the original QGroupBox::toggled handler
/// that forced the button group back to its first button. See AdvancedMeshesModule::settings_to_ui
/// (unchanged) for why that mattered there.
class AdvancedMeshesBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool baseChecked READ baseChecked WRITE setBaseChecked NOTIFY baseCheckedChanged)
    Q_PROPERTY(bool fullOptimization READ fullOptimization WRITE setFullOptimization NOTIFY
                   fullOptimizationChanged)

public:
    using QObject::QObject;

    [[nodiscard]] auto baseChecked() const -> bool;
    void setBaseChecked(bool value);

    [[nodiscard]] auto fullOptimization() const -> bool;
    void setFullOptimization(bool value);

signals:
    void baseCheckedChanged();
    void fullOptimizationChanged();

private:
    bool base_checked_      = false;
    bool full_optimization_ = false;
};

} // namespace cao
