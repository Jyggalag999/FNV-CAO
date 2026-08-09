/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "LevelSelector.hpp"

#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {

LevelSelector::LevelSelector(GuiSettings settings)
    : QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
    , bridge_(settings.gui_mode)
    , settings_(std::move(settings))
{
    setWindowTitle(tr("Level Selector"));
    resize(570, 150);

    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/LevelSelector.qml"));
    layout->addWidget(qml_widget_);

    connect(&bridge_, &LevelSelectorBridge::modeSelected, this, &QDialog::accept);
}

auto LevelSelector::run_selection() noexcept -> GuiSettings
{
    if (QDialog::exec() != Accepted)
        return settings_;

    settings_.gui_mode          = bridge_.selectedMode();
    settings_.remember_gui_mode = bridge_.rememberChoice();

    return settings_;
}

} // namespace cao
