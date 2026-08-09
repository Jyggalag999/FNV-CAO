/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "LevelSelectorBridge.hpp"
#include "settings/settings.hpp"

#include <QDialog>

class QQuickWidget;

namespace cao {

class LevelSelector final : public QDialog
{
    Q_OBJECT

public:
    explicit LevelSelector(GuiSettings settings);

    LevelSelector(const LevelSelector &) = delete;
    LevelSelector(LevelSelector &&)      = delete;

    auto operator=(const LevelSelector &) -> LevelSelector & = delete;
    auto operator=(LevelSelector &&) -> LevelSelector      & = delete;

    ~LevelSelector() override = default;

    [[nodiscard]] auto run_selection() noexcept -> GuiSettings;

private:
    LevelSelectorBridge bridge_;
    GuiSettings settings_;
    QQuickWidget *qml_widget_ = nullptr;
};
} // namespace cao
