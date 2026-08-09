/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProgressWindow.hpp"

#include "utils/utils.hpp"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>
#include <utility>

namespace cao {

ProgressWindow::ProgressWindow(LogReader log_reader, QWidget *parent)
    : QWidget(parent)
    , log_reader_(std::move(log_reader))
{
    setWindowTitle(tr("Progress"));
    resize(546, 534);
    setWindowModality(Qt::WindowModal); // Prevents user from interacting with the main window

    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_widget_->rootContext()->setContextProperty("logModel", &model_);
    qml_widget_->setSource(QUrl("qrc:/qml/ProgressWindow.qml"));
    layout->addWidget(qml_widget_);

    connect(&model_, &ProgressLogModel::openLogFileRequested, this, [this] {
        const auto path = to_qstring(btu::fs::absolute(log_reader_.get_log_path()).u8string());
        QDesktopServices::openUrl(QUrl("file:///" + path));
    });

    timer_.setSingleShot(false);
    timer_.start(1000);
    connect(&timer_, &QTimer::timeout, this, &ProgressWindow::update_all);
}

ProgressWindow::~ProgressWindow()
{
    end();
}

void ProgressWindow::set_maximum(int max)
{
    model_.setProgressMaximum(max);
}

void ProgressWindow::step(std::optional<QString> text, int n)
{
    current_value_ += n;
    last_text_ = std::move(text);
}

void ProgressWindow::end()
{
    if (model_.progressMaximum() == 0)
        model_.setProgressMaximum(1); // avoid busy indicator

    update_all();

    model_.setProgressValue(model_.progressMaximum());
    model_.setProgressText(tr("Done"));

    timer_.stop();
}

void ProgressWindow::update_all()
{
    if (last_text_)
        update_progress_bar(*last_text_, model_.progressMaximum(), current_value_);
    else
        model_.setProgressValue(current_value_);

    update_log();
}

void ProgressWindow::update_progress_bar(const QString &text, int max, int value)
{
    const int percent = max > 0 ? (value * 100 / max) : 0;
    model_.setProgressText(QString("%1 %2%").arg(text).arg(percent));
    model_.setProgressMaximum(max);
    model_.setProgressValue(value);
}

void ProgressWindow::update_log()
{
    model_.add_entries(log_reader_.update());
}

[[maybe_unused]] void ProgressWindow::closeEvent(QCloseEvent *event)
{
    emit cancelled();
    event->accept();
}

} // namespace cao
