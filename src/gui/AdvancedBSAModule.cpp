/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedBSAModule.hpp"

#include <QColor>
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
    // QML content is content-sized now, not full-page (see AdvancedBSAModule.qml /
    // GeneralBSAModule.cpp for the full rationale). Was pinned to a hardcoded opaque match for
    // QTabWidget::pane's background instead of real transparency - now that pane is translucent
    // (nebula_overrides in MainWindow.cpp, so the nebula photo behind everything actually shows),
    // this needs the same WA_AlwaysStackOnTop + transparent clear color fix as top_bar_widget_.
    qml_widget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    qml_widget_->setClearColor(Qt::transparent);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/AdvancedBSAModule.qml"));
    layout->addWidget(qml_widget_);
}

void AdvancedBSAModule::settings_to_ui(const Settings &settings)
{
    // Global override now (Profile::force_pack_always), not tied to the selected pattern - see
    // AdvancedTexturesModule for the same treatment of textures.
    bridge_.setPackFile(settings.current_profile().force_pack_always);
}

void AdvancedBSAModule::ui_to_settings(Settings &settings) const
{
    settings.current_profile().force_pack_always = bridge_.packFile();
}

auto AdvancedBSAModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    // FNV-only fork: the profile picker (ProfilesManagerWindow::k_games) never offers a
    // non-FNV game anyway, so this is purely defensive - see settings.cpp's make_base() comment.
    return game == btu::Game::FNV;
}

auto AdvancedBSAModule::name() const noexcept -> QString
{
    return QObject::tr("BSA (Patterns)");
}

} // namespace cao
