/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedTexturesModule.hpp"

#include "settings/per_file_settings.hpp"

#include <btu/common/algorithms.hpp>

#include <QColor>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {
AdvancedTexturesModule::AdvancedTexturesModule(QWidget *parent)
    : IWindowModule(parent)
{
    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)
    layout->setContentsMargins(0, 0, 0, 0);

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // QML content is content-sized now, not full-page (see AdvancedTexturesModule.qml /
    // GeneralBSAModule.cpp for the full rationale) - matches QTabWidget::pane's own background
    // instead of defaulting to QQuickWidget's white clear color for the leftover space.
    qml_widget_->setClearColor(QColor("#0d0818"));
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/AdvancedTexturesModule.qml"));
    layout->addWidget(qml_widget_);
}

auto AdvancedTexturesModule::name() const noexcept -> QString
{
    return tr("Textures (Patterns)");
}

void AdvancedTexturesModule::settings_to_ui(const Settings &settings)
{
    const auto &pfs = current_per_file_settings(settings);

    // Main
    bridge_.setMainChecked(pfs.tex_optimize != OptimizeType::None);
    bridge_.setCompress(pfs.tex.compress);
    bridge_.setMipmaps(pfs.tex.mipmaps);
    bridge_.setForceCrunch(settings.current_profile().force_crunch_always);

    // Resizing
    bridge_.setResizingChecked(true);
    std::visit(btu::common::Overload{
                   [this](std::monostate) { bridge_.setResizingChecked(false); },
                   [this](btu::tex::util::ResizeRatio ratio) {
                       bridge_.setResizeByRatio(true);
                       bridge_.setWidth(static_cast<int>(ratio.ratio));
                       bridge_.setHeight(static_cast<int>(ratio.ratio));
                       bridge_.setMinimumChecked(true);
                       bridge_.setMinimumWidth(static_cast<int>(ratio.min.w));
                       bridge_.setMinimumHeight(static_cast<int>(ratio.min.h));
                   },
                   [this](btu::tex::Dimension dim) {
                       bridge_.setResizeByRatio(false);
                       bridge_.setWidth(static_cast<int>(dim.w));
                       bridge_.setHeight(static_cast<int>(dim.h));
                       bridge_.setMinimumChecked(false);
                   }},
               pfs.tex.resize);
}

void AdvancedTexturesModule::ui_to_settings(Settings &settings) const
{
    auto &pfs = current_per_file_settings(settings);

    // Main
    pfs.tex_optimize                                = bridge_.mainChecked() ? OptimizeType::Normal
                                                                              : OptimizeType::None;
    pfs.tex.compress                                = bridge_.compress();
    pfs.tex.mipmaps                                 = bridge_.mipmaps();
    settings.current_profile().force_crunch_always  = bridge_.forceCrunch();

    // Resizing
    pfs.tex.resize = std::monostate{};
    if (bridge_.resizingChecked())
    {
        if (bridge_.resizeByRatio())
        {
            pfs.tex.resize = btu::tex::util::ResizeRatio{
                .ratio = static_cast<uint8_t>(bridge_.width()),
                .min   = bridge_.minimumChecked()
                             ? btu::tex::Dimension{static_cast<size_t>(bridge_.minimumWidth()),
                                                   static_cast<size_t>(bridge_.minimumHeight())}
                             : btu::tex::Dimension{0, 0}};
        }
        else
        {
            pfs.tex.resize = btu::tex::Dimension{.w = static_cast<size_t>(bridge_.width()),
                                                 .h = static_cast<size_t>(bridge_.height())};
        }
    }
}

auto AdvancedTexturesModule::is_supported_game(btu::Game /*game*/) const noexcept -> bool
{
    return true; // even if the game is not supported, the module is probably still useful
}

} // namespace cao
