/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "plog/Severity.h"

#include <btu/common/path.hpp>

#include <QMetaType>
#include <QString>

#include <vector>

class QTextStream;

Q_DECLARE_METATYPE(plog::Severity)

namespace cao {

/// @brief Reads new lines appended to CAO's own log file since the last call - used to stream the
/// running process's log into ProgressWindow. Split out from ProgressWindow.hpp (Step 4 QML port)
/// so ProgressLogModel can use LogEntry without a circular include between the two.
class LogReader
{
public:
    struct LogEntry
    {
        explicit LogEntry(QString line);
        LogEntry(QString line, plog::Severity severity);

        QString text;
        plog::Severity severity;
    };

    explicit LogReader(btu::Path log_file_path);

    [[nodiscard]] auto update() -> std::vector<LogEntry>;

    [[nodiscard]] auto get_log_path() const noexcept -> btu::Path;

private:
    void advance_to_last_read(QTextStream &log_stream);
    void update_last_read(QTextStream &ts);

    [[nodiscard]] static auto read_line(QTextStream &log_stream) -> LogEntry;

    QString current_file_first_line_;
    std::streampos log_read_pos_;

    btu::Path log_file_path_;
};

} // namespace cao
