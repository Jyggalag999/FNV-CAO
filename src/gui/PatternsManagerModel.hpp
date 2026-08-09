/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QAbstractListModel>
#include <QString>

#include <utility>
#include <vector>

namespace cao {

/// @brief Pure display projection of the current profile's patterns (excluding the default
/// pattern, same filter update_patterns() always applied) for QML's ListView. Holds no Settings
/// reference and does no mutation itself - PatternsManagerWindow still owns all the real
/// Settings::current_profile() editing, exactly as before, reacting to the request* signals below
/// and calling set_patterns() afterward to refresh the display. Same split of responsibility as
/// ListDialogModel/ProgressLogModel.
class PatternsManagerModel final : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles : int
    {
        TextRole = Qt::UserRole + 1,
        RegexRole,
    };

    using QAbstractListModel::QAbstractListModel;

    [[nodiscard]] auto rowCount(const QModelIndex &parent = {}) const -> int override;
    [[nodiscard]] auto data(const QModelIndex &index, int role) const -> QVariant override;
    [[nodiscard]] auto roleNames() const -> QHash<int, QByteArray> override;

    /// Replaces the displayed list. `patterns` is {text, isRegex} pairs, in display order.
    void set_patterns(std::vector<std::pair<QString, bool>> patterns);

    Q_INVOKABLE void requestNew();
    Q_INVOKABLE void requestRemove(int row);
    Q_INVOKABLE void requestRename(int row, const QString &newText);
    Q_INVOKABLE void requestMoveUp(int row);
    Q_INVOKABLE void requestMoveDown(int row);

signals:
    void newRequested();
    void removeRequested(int row);
    void renameRequested(int row, QString newText);

    /// sourceRow/destRow use PerFileSettings::move_per_file_settings()'s own index convention
    /// (destRow is where the item ends up, already accounting for the removal shift) - see
    /// PatternsManagerWindow::move_pattern.
    void moveRequested(int sourceRow, int destRow);

private:
    struct Entry
    {
        QString text;
        bool is_regex;
    };

    std::vector<Entry> entries_;
};

} // namespace cao
