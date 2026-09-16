/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedMeshesModule.hpp"

#include <QColor>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {
AdvancedMeshesModule::AdvancedMeshesModule(QWidget *parent)
    : IWindowModule(parent)
{
    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)
    layout->setContentsMargins(0, 0, 0, 0);

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // QML content is content-sized now, not full-page (see AdvancedMeshesModule.qml /
    // GeneralBSAModule.cpp for the full rationale). Was pinned to a hardcoded opaque match for
    // QTabWidget::pane's background instead of real transparency - now that pane is translucent
    // (nebula_overrides in MainWindow.cpp, so the nebula photo behind everything actually shows),
    // this needs the same WA_AlwaysStackOnTop + transparent clear color fix as top_bar_widget_.
    qml_widget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    qml_widget_->setClearColor(Qt::transparent);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/AdvancedMeshesModule.qml"));
    layout->addWidget(qml_widget_);
}

auto AdvancedMeshesModule::name() const noexcept -> QString
{
    return tr("Meshes (Patterns)");
}

void AdvancedMeshesModule::settings_to_ui(const Settings &settings)
{
    switch (current_per_file_settings(settings).nif_optimize)
    {
        case OptimizeType::None:
            bridge_.setBaseChecked(false);
            break;
        case OptimizeType::DryRun:
        case OptimizeType::Normal:
            bridge_.setBaseChecked(true);
            bridge_.setFullOptimization(false);
            break;
        case OptimizeType::Forced:
            bridge_.setBaseChecked(true);
            bridge_.setFullOptimization(true);
            break;
    }
}

void AdvancedMeshesModule::ui_to_settings(Settings &settings) const
{
    auto &pfs       = current_per_file_settings(settings);
    const bool base = bridge_.baseChecked();

    if (base && !bridge_.fullOptimization())
        pfs.nif_optimize = OptimizeType::Normal;
    else if (base && bridge_.fullOptimization())
    {
        pfs.nif_optimize = OptimizeType::Forced;
        pfs.nif.optimize = true;
    }
    else
        pfs.nif_optimize = OptimizeType::None;
}

auto AdvancedMeshesModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    switch (game)
    {
        // all these games appear to be supported here
        // <https://github.com/ousnius/nifly/blob/868b648b72392d3f8f582a28f819253dc5c6063e/include/BasicTypes.hpp#L150>
        case btu::Game::TES4:
        case btu::Game::SLE:
        case btu::Game::SSE:
        case btu::Game::FO4:
        case btu::Game::Starfield: return true;
        case btu::Game::TES3:
        case btu::Game::FNV:
        case btu::Game::Custom: return false;
    }
    return false;
}

} // namespace cao
