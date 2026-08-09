/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "LogReader.hpp"

#include <btu/common/algorithms.hpp>
#include <btu/common/metaprogramming.hpp>

#include <QFile>
#include <QTextStream>
#include <utility>

namespace cao {

LogReader::LogReader(btu::Path log_file_path)
    : log_file_path_(BTU_MOV(log_file_path))
{
}

auto LogReader::update() -> std::vector<LogEntry>
{
    // We have to reopen the file every time because it can be rotated
    QFile log_file(log_file_path_);
    log_file.open(QFile::ReadOnly);

    if (!log_file.isOpen())
        return {{QString("Failed to open log file: %1").arg(QString::fromStdString(log_file_path_.string())),
                 plog::error}};

    auto log_stream = QTextStream(&log_file);
    advance_to_last_read(log_stream);

    auto entries = std::vector<LogEntry>{};

    while (!log_stream.atEnd())
        entries.emplace_back(read_line(log_stream));

    update_last_read(log_stream);

    return entries;
}

auto LogReader::get_log_path() const noexcept -> btu::Path
{
    return log_file_path_;
}

auto LogReader::read_line(QTextStream &log_stream) -> LogEntry
{
    constexpr char log_line_end = '|';

    QString log_line = log_stream.readLine();
    log_line.reserve(1000);

    while (true)
    {
        if (log_stream.atEnd())
            break;

        if (log_line.endsWith(log_line_end))
        {
            log_line.chop(1); // Remove `log_line_end`
            break;
        }

        log_line.append(log_stream.readLine());
    }
    return LogEntry(BTU_MOV(log_line));
}

void LogReader::update_last_read(QTextStream &ts)
{
    log_read_pos_ = ts.pos();
}

void LogReader::advance_to_last_read(QTextStream &log_stream)
{
    auto first = log_stream.readLine();

    if (first != current_file_first_line_) // Current and previous file are not the same
    {
        current_file_first_line_ = std::move(first);
        log_read_pos_            = 0;
    }

    log_stream.seek(log_read_pos_);
}

LogReader::LogEntry::LogEntry(QString line)
    : text(BTU_MOV(line))
{
    // We know our CustomFormatter is used. Time always uses the same number of chars, 24
    constexpr int time_length = 24;
    const auto level_end      = text.indexOf(' ', time_length) - time_length;
    const auto line_severity  = text.mid(time_length, level_end);

    severity = plog::severityFromString(qPrintable(line_severity));
}
LogReader::LogEntry::LogEntry(QString line, plog::Severity severity)
    : text(BTU_MOV(line))
    , severity(severity)
{
}

} // namespace cao
