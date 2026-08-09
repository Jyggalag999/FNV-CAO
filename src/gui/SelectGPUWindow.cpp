/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "SelectGPUWindow.hpp"

#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {
using btu::tex::CompressionDevice;

SelectGPUWindow::SelectGPUWindow(QWidget *parent)
    : QDialog(parent)
    , bridge_(CompressionDevice().list_adapters())
{
    setWindowTitle(tr("Select GPU"));
    resize(400, 300);

    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/SelectGpuDialog.qml"));
    layout->addWidget(qml_widget_);

    connect(&bridge_, &SelectGpuBridge::accepted, this, &QDialog::accept);
    connect(&bridge_, &SelectGpuBridge::rejected, this, &QDialog::reject);
}

auto SelectGPUWindow::get_selected_index() -> std::optional<uint32_t>
{
    const int idx = bridge_.selectedIndex();
    if (idx < 0)
        return std::nullopt;

    return static_cast<uint32_t>(idx);
}

void SelectGPUWindow::set_selected_index(uint32_t val)
{
    bridge_.setSelectedIndex(static_cast<int>(val));
}
} // namespace cao
