/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "AdvancedMeshesBridge.hpp"
#include "IWindowModule.hpp"

class QQuickWidget;

namespace cao {
class AdvancedMeshesModule final : public IWindowModule
{
    Q_OBJECT
public:
    explicit AdvancedMeshesModule(QWidget *parent = nullptr);

    AdvancedMeshesModule(const AdvancedMeshesModule &)                     = delete;
    auto operator=(const AdvancedMeshesModule &) -> AdvancedMeshesModule & = delete;

    AdvancedMeshesModule(AdvancedMeshesModule &&)                     = delete;
    auto operator=(AdvancedMeshesModule &&) -> AdvancedMeshesModule & = delete;

    ~AdvancedMeshesModule() override = default;

    [[nodiscard]] auto name() const noexcept -> QString override;

    void ui_to_settings(Settings &settings) const override;

private:
    AdvancedMeshesBridge bridge_;
    QQuickWidget *qml_widget_ = nullptr;

    void settings_to_ui(const Settings &settings) override;
    [[nodiscard]] auto is_supported_game(btu::Game game) const noexcept -> bool override;
};
} // namespace cao
