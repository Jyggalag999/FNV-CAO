/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "ProgressLogModel.hpp"

namespace cao {

constexpr int k_max_log_entries = 5000;

[[nodiscard]] auto log_color(plog::Severity severity) noexcept -> const char *
{
    switch (severity)
    {
        case plog::none: break;
        case plog::fatal: return "DarkRed";
        case plog::error: return "Red";
        case plog::warning: return "Orange";
        case plog::info: return "Green";
        case plog::debug: return "Blue";
        case plog::verbose: return "Purple";
    }
    return "Black";
}

ProgressLogModel::ProgressLogModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

auto ProgressLogModel::rowCount(const QModelIndex &parent) const -> int
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(entries_.size());
}

auto ProgressLogModel::data(const QModelIndex &index, int role) const -> QVariant
{
    if (!index.isValid() || static_cast<size_t>(index.row()) >= entries_.size())
        return {};

    const auto &entry = entries_[static_cast<size_t>(index.row())];
    switch (role)
    {
        case TextRole: return entry.text;
        case ColorRole: return QString(log_color(entry.severity));
        case HiddenRole: return is_hidden(entry.severity);
        default: return {};
    }
}

auto ProgressLogModel::roleNames() const -> QHash<int, QByteArray>
{
    return {
        {TextRole, "text"},
        {ColorRole, "color"},
        {HiddenRole, "hidden"},
    };
}

void ProgressLogModel::add_entries(const std::vector<LogReader::LogEntry> &entries)
{
    if (entries.empty())
        return;

    const auto first_row = static_cast<int>(entries_.size());
    beginInsertRows({}, first_row, first_row + static_cast<int>(entries.size()) - 1);
    for (const auto &entry : entries)
        entries_.push_back({entry.text, entry.severity});
    endInsertRows();

    if (entries_.size() > static_cast<size_t>(k_max_log_entries))
    {
        const auto remove_count = k_max_log_entries / 2;
        beginRemoveRows({}, 0, remove_count - 1);
        entries_.erase(entries_.begin(), entries_.begin() + remove_count);
        endRemoveRows();
    }
}

auto ProgressLogModel::progressMaximum() const -> int
{
    return progress_maximum_;
}

void ProgressLogModel::setProgressMaximum(int value)
{
    if (progress_maximum_ == value)
        return;

    progress_maximum_ = value;
    emit progressMaximumChanged();
}

auto ProgressLogModel::progressValue() const -> int
{
    return progress_value_;
}

void ProgressLogModel::setProgressValue(int value)
{
    if (progress_value_ == value)
        return;

    progress_value_ = value;
    emit progressValueChanged();
}

auto ProgressLogModel::progressText() const -> QString
{
    return progress_text_;
}

void ProgressLogModel::setProgressText(const QString &value)
{
    if (progress_text_ == value)
        return;

    progress_text_ = value;
    emit progressTextChanged();
}

auto ProgressLogModel::isLevelVerbose() const -> bool
{
    return log_level_ == plog::verbose;
}

auto ProgressLogModel::isLevelInfo() const -> bool
{
    return log_level_ == plog::info;
}

auto ProgressLogModel::isLevelError() const -> bool
{
    return log_level_ == plog::error;
}

void ProgressLogModel::selectLevelVerbose()
{
    set_log_level(plog::verbose);
}

void ProgressLogModel::selectLevelInfo()
{
    set_log_level(plog::info);
}

void ProgressLogModel::selectLevelError()
{
    set_log_level(plog::error);
}

void ProgressLogModel::set_log_level(plog::Severity level)
{
    if (log_level_ == level)
        return;

    log_level_ = level;
    emit logLevelChanged();

    if (!entries_.empty())
        emit dataChanged(index(0), index(static_cast<int>(entries_.size()) - 1), {HiddenRole});
}

auto ProgressLogModel::is_hidden(plog::Severity severity) const -> bool
{
    return severity > log_level_;
}

void ProgressLogModel::clearLog()
{
    if (entries_.empty())
        return;

    beginResetModel();
    entries_.clear();
    endResetModel();
}

void ProgressLogModel::requestOpenLogFile()
{
    emit openLogFileRequested();
}

} // namespace cao
