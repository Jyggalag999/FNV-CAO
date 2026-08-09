/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "LogReader.hpp"

#include <QAbstractListModel>
#include <QString>

#include <vector>

namespace cao {

/// @brief Owns ProgressWindow's log entries and exposes them to QML's ListView, plus the
/// progress bar's state (all pre-formatted in C++ - see ProgressWindow::update_progress_bar/end,
/// unchanged in spirit from the old QProgressBar::setFormat() calls) and the log-level filter.
///
/// Log level is exposed as three named booleans + three named selectors (levelVerbose/
/// levelInfo/levelError, selectLevelVerbose/...), same reasoning as LevelSelectorBridge's
/// per-mode methods: plog::Severity isn't QML-visible, and there are only three named levels in
/// the old combo box, so this avoids adding enum-marshalling machinery for no real benefit.
class ProgressLogModel final : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int progressMaximum READ progressMaximum WRITE setProgressMaximum NOTIFY
                   progressMaximumChanged)
    Q_PROPERTY(
        int progressValue READ progressValue WRITE setProgressValue NOTIFY progressValueChanged)
    Q_PROPERTY(
        QString progressText READ progressText WRITE setProgressText NOTIFY progressTextChanged)

    Q_PROPERTY(bool levelVerbose READ isLevelVerbose NOTIFY logLevelChanged)
    Q_PROPERTY(bool levelInfo READ isLevelInfo NOTIFY logLevelChanged)
    Q_PROPERTY(bool levelError READ isLevelError NOTIFY logLevelChanged)

public:
    enum Roles : int
    {
        TextRole = Qt::UserRole + 1,
        ColorRole,
        HiddenRole,
    };

    explicit ProgressLogModel(QObject *parent = nullptr);

    [[nodiscard]] auto rowCount(const QModelIndex &parent = {}) const -> int override;
    [[nodiscard]] auto data(const QModelIndex &index, int role) const -> QVariant override;
    [[nodiscard]] auto roleNames() const -> QHash<int, QByteArray> override;

    /// Appends new entries (from LogReader::update()) and trims to k_max_log_entries, same as the
    /// old update_log()'s QListWidget management.
    void add_entries(const std::vector<LogReader::LogEntry> &entries);

    [[nodiscard]] auto progressMaximum() const -> int;
    void setProgressMaximum(int value);

    [[nodiscard]] auto progressValue() const -> int;
    void setProgressValue(int value);

    [[nodiscard]] auto progressText() const -> QString;
    void setProgressText(const QString &value);

    [[nodiscard]] auto isLevelVerbose() const -> bool;
    [[nodiscard]] auto isLevelInfo() const -> bool;
    [[nodiscard]] auto isLevelError() const -> bool;

    Q_INVOKABLE void selectLevelVerbose();
    Q_INVOKABLE void selectLevelInfo();
    Q_INVOKABLE void selectLevelError();

    Q_INVOKABLE void clearLog();
    Q_INVOKABLE void requestOpenLogFile();

signals:
    void progressMaximumChanged();
    void progressValueChanged();
    void progressTextChanged();
    void logLevelChanged();

    /// QML's "Open log file" button asks ProgressWindow (which owns the LogReader/path) to open
    /// it - a plain QDesktopServices call, not something the model needs to know the path for.
    void openLogFileRequested();

private:
    struct Entry
    {
        QString text;
        plog::Severity severity;
    };

    [[nodiscard]] auto is_hidden(plog::Severity severity) const -> bool;
    void set_log_level(plog::Severity level);

    std::vector<Entry> entries_;
    QString progress_text_;
    int progress_maximum_  = 0;
    int progress_value_    = 0;
    plog::Severity log_level_ = plog::info;
};

} // namespace cao
