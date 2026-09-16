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
    // GeneralBSAModule.cpp for the full rationale). Was pinned to a hardcoded opaque match for
    // QTabWidget::pane's background instead of real transparency - now that pane is translucent
    // (nebula_overrides in MainWindow.cpp, so the nebula photo behind everything actually shows),
    // this needs the same WA_AlwaysStackOnTop + transparent clear color fix as top_bar_widget_.
    qml_widget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    qml_widget_->setClearColor(Qt::transparent);
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
    const auto &profile = settings.current_profile();

    // All global overrides now (Profile::force_*), independent of whichever pattern is selected -
    // resize included, as of force_resize: it used to be the one exception here, silently read
    // from whatever pattern the Pattern dropdown happened to have selected rather than being a
    // real global setting.
    bridge_.setMainChecked(profile.force_process_textures);
    bridge_.setCompress(profile.force_compress_always);
    bridge_.setCompressUncompressedOnly(profile.force_compress_uncompressed_only);
    bridge_.setMipmaps(profile.force_mipmaps_always);
    bridge_.setForceCrunch(profile.force_crunch_always);

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
               profile.force_resize);
}

void AdvancedTexturesModule::ui_to_settings(Settings &settings) const
{
    auto &profile = settings.current_profile();

    // Main - see settings_to_ui(): all global overrides, not tied to the selected pattern.
    profile.force_process_textures           = bridge_.mainChecked();
    profile.force_compress_always            = bridge_.compress();
    profile.force_compress_uncompressed_only = bridge_.compressUncompressedOnly();
    profile.force_mipmaps_always             = bridge_.mipmaps();
    profile.force_crunch_always              = bridge_.forceCrunch();

    // Resizing
    profile.force_resize = std::monostate{};
    if (bridge_.resizingChecked())
    {
        if (bridge_.resizeByRatio())
        {
            profile.force_resize = btu::tex::util::ResizeRatio{
                .ratio = static_cast<uint8_t>(bridge_.width()),
                .min   = bridge_.minimumChecked()
                             ? btu::tex::Dimension{static_cast<size_t>(bridge_.minimumWidth()),
                                                   static_cast<size_t>(bridge_.minimumHeight())}
                             : btu::tex::Dimension{0, 0}};
        }
        else
        {
            profile.force_resize = btu::tex::Dimension{.w = static_cast<size_t>(bridge_.width()),
                                                       .h = static_cast<size_t>(bridge_.height())};
        }
    }
}

auto AdvancedTexturesModule::is_supported_game(btu::Game /*game*/) const noexcept -> bool
{
    return true; // even if the game is not supported, the module is probably still useful
}

} // namespace cao
