/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "SelectGpuBridge.hpp"

#include <QDialog>
#include <optional>

class QQuickWidget;

namespace cao {

/// @brief GPU picker dialog. Step 3 pattern-proof: internals are fully QML now (see
/// SelectGpuBridge.hpp and src/gui/qml/SelectGpuDialog.qml) - the old Widgets/.ui implementation
/// is gone. Public API is unchanged from before, so MainWindow's call site didn't need to change.
class SelectGPUWindow final : public QDialog
{
    Q_OBJECT

public:
    explicit SelectGPUWindow(QWidget *parent = nullptr);

    [[nodiscard]] auto get_selected_index() -> std::optional<uint32_t>;
    void set_selected_index(uint32_t val);

private:
    SelectGpuBridge bridge_;
    QQuickWidget *qml_widget_ = nullptr;
};
} // namespace cao
