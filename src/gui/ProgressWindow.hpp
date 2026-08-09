/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "LogReader.hpp"
#include "ProgressLogModel.hpp"

#include <QTimer>
#include <QWidget>

class QCloseEvent;
class QQuickWidget;

namespace cao {

class ProgressWindow final : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWindow(LogReader log_reader, QWidget *parent = nullptr);

    ProgressWindow(const ProgressWindow &)                     = delete;
    auto operator=(const ProgressWindow &) -> ProgressWindow & = delete;

    ProgressWindow(ProgressWindow &&)                     = delete;
    auto operator=(ProgressWindow &&) -> ProgressWindow & = delete;

    ~ProgressWindow() override; // = default

    void set_maximum(int max);
    void step(std::optional<QString> text = std::nullopt, int n = 1);
    void end();

signals:
    void cancelled();

protected:
    [[maybe_unused]] void closeEvent(QCloseEvent *event) override;

private:
    int current_value_{0};
    std::optional<QString> last_text_;
    QTimer timer_;

    LogReader log_reader_;
    ProgressLogModel model_;
    QQuickWidget *qml_widget_ = nullptr;

    void update_all();

    void update_progress_bar(const QString &text, int max, int value);
    void update_log();
};
} // namespace cao
