/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/base_types.hpp"

#include <QObject>
#include <QString>

namespace cao {

/// @brief Thin QML-facing adapter for LevelSelector. GuiMode isn't a QObject/Q_ENUM anywhere else
/// in the app, so rather than add that machinery just for this one dialog, each of the three
/// modes gets its own explicit hover*/select* method - more verbose than a single
/// hover(int)/select(int) pair, but keeps GuiMode itself out of QML entirely.
class LevelSelectorBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString helpText READ helpText NOTIFY helpTextChanged)
    Q_PROPERTY(
        bool rememberChoice READ rememberChoice WRITE setRememberChoice NOTIFY rememberChoiceChanged)

public:
    explicit LevelSelectorBridge(GuiMode initial_mode, QObject *parent = nullptr);

    [[nodiscard]] auto helpText() const -> QString;

    [[nodiscard]] auto rememberChoice() const -> bool;
    void setRememberChoice(bool value);

    /// The mode from the most recent select*() call - read by LevelSelector once modeSelected()
    /// fires. Not a Q_PROPERTY since GuiMode isn't QML-visible.
    [[nodiscard]] auto selectedMode() const -> GuiMode;

    Q_INVOKABLE void hoverQuickOptimize();
    Q_INVOKABLE void hoverMedium();
    Q_INVOKABLE void hoverAdvanced();

    Q_INVOKABLE void selectQuickOptimize();
    Q_INVOKABLE void selectMedium();
    Q_INVOKABLE void selectAdvanced();

signals:
    void helpTextChanged();
    void rememberChoiceChanged();

    /// A mode button was clicked; LevelSelector reads selectedMode() and accepts the dialog.
    void modeSelected();

private:
    void set_help_text(GuiMode mode);
    void select(GuiMode mode);

    GuiMode selected_mode_;
    QString help_text_;
    bool remember_choice_ = false;
};

} // namespace cao
