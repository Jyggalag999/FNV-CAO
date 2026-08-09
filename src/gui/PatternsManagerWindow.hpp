/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "PatternsManagerModel.hpp"
#include "settings/profile.hpp"
#include "settings/settings.hpp"

#include <QDialog>

class QQuickWidget;

namespace cao {
class Settings;

class PatternsManagerWindow final : public QDialog
{
    Q_OBJECT

public:
    explicit PatternsManagerWindow(Settings &settings, QWidget *parent = nullptr);

    PatternsManagerWindow(const PatternsManagerWindow &) = delete;
    PatternsManagerWindow(PatternsManagerWindow &&)      = delete;

    auto operator=(const PatternsManagerWindow &) -> PatternsManagerWindow & = delete;
    auto operator=(PatternsManagerWindow &&) -> PatternsManagerWindow      & = delete;

    ~PatternsManagerWindow() override = default;

private:
    PatternsManagerModel model_;
    Settings &settings_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    QQuickWidget *qml_widget_ = nullptr;

    /// Patterns currently configured for the active profile, excluding the default pattern - same
    /// filter and same order the model is populated in, so row indices from the model always
    /// index directly into this.
    [[nodiscard]] auto displayed_patterns() const -> std::vector<PerFileSettings *>;

    void update_patterns();
    void update_pattern(int row, const QString &new_text);
    void move_pattern(int source_row, int destination_row);
    void create_pattern();
    void delete_pattern(int row);
};
} // namespace cao
