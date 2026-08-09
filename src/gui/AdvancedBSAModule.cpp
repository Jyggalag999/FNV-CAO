/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedBSAModule.hpp"

#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {
AdvancedBSAModule::AdvancedBSAModule(QWidget *parent)
    : IWindowModule(parent)
{
    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)
    layout->setContentsMargins(0, 0, 0, 0);

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/AdvancedBSAModule.qml"));
    layout->addWidget(qml_widget_);
}

void AdvancedBSAModule::settings_to_ui(const Settings &settings)
{
    auto &pfs = current_per_file_settings(settings);
    bridge_.setPackFile(pfs.pack);
}

void AdvancedBSAModule::ui_to_settings(Settings &settings) const
{
    auto &pfs = current_per_file_settings(settings);
    pfs.pack  = bridge_.packFile();
}

auto AdvancedBSAModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    switch (game)
    {
        case btu::Game::TES3:
        case btu::Game::TES4:
        case btu::Game::SLE:
        case btu::Game::SSE:
        case btu::Game::FNV:
        case btu::Game::FO4:
        case btu::Game::Starfield: return true;
        case btu::Game::Custom: return false;
    }
    return false;
}

auto AdvancedBSAModule::name() const noexcept -> QString
{
    return QObject::tr("BSA (Patterns)");
}

} // namespace cao
