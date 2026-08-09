/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "PatternsManagerModel.hpp"

namespace cao {

auto PatternsManagerModel::rowCount(const QModelIndex &parent) const -> int
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(entries_.size());
}

auto PatternsManagerModel::data(const QModelIndex &index, int role) const -> QVariant
{
    if (!index.isValid() || static_cast<size_t>(index.row()) >= entries_.size())
        return {};

    const auto &entry = entries_[static_cast<size_t>(index.row())];
    switch (role)
    {
        case TextRole: return entry.text;
        case RegexRole: return entry.is_regex;
        default: return {};
    }
}

auto PatternsManagerModel::roleNames() const -> QHash<int, QByteArray>
{
    return {
        {TextRole, "text"},
        {RegexRole, "isRegex"},
    };
}

void PatternsManagerModel::set_patterns(std::vector<std::pair<QString, bool>> patterns)
{
    beginResetModel();
    entries_.clear();
    entries_.reserve(patterns.size());
    for (auto &[text, is_regex] : patterns)
        entries_.push_back({std::move(text), is_regex});
    endResetModel();
}

void PatternsManagerModel::requestNew()
{
    emit newRequested();
}

void PatternsManagerModel::requestRemove(int row)
{
    if (row < 0 || static_cast<size_t>(row) >= entries_.size())
        return;

    emit removeRequested(row);
}

void PatternsManagerModel::requestRename(int row, const QString &newText)
{
    if (row < 0 || static_cast<size_t>(row) >= entries_.size())
        return;

    emit renameRequested(row, newText);
}

void PatternsManagerModel::requestMoveUp(int row)
{
    if (row <= 0 || static_cast<size_t>(row) >= entries_.size())
        return;

    emit moveRequested(row, row - 1);
}

void PatternsManagerModel::requestMoveDown(int row)
{
    if (row < 0 || static_cast<size_t>(row) + 1 >= entries_.size())
        return;

    // +2: move_per_file_settings() uses Qt's rowsMoved destinationRow convention (an index into
    // the list as it stood before the source row was removed) - see PatternsManagerModel.hpp.
    emit moveRequested(row, row + 2);
}

} // namespace cao
