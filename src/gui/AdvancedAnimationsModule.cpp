/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "AdvancedAnimationsModule.hpp"

#include <QColor>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {

// Every btu::kf::Settings field below is hardcoded to the known-good/vanilla-matching values
// rather than exposed as a UI toggle - the whole point of kfcompress (see F:\Kompresor CLI's
// PROJECT_SUMMARY.md) was reconstructing FNV's actual vanilla animation compression, verified
// against 553 real vanilla-compressed files and a real 14,313-file mod batch; letting these drift
// away from what was actually verified just reintroduces the risk that reconstruction eliminated.
// The only user-facing control is whether animations get compressed at all (bridge_.enabled()).
namespace {
constexpr double k_vanilla_matching_ratio = 0.80; // see PROJECT_SUMMARY.md: exact spike at 0.800
                                                  // in 40% of 553 real vanilla-compressed files
constexpr bool k_compact16              = true;  // pack control points as 16-bit fixed point,
                                                  // matching NiBSplineCompTransformInterpolator
constexpr bool k_use_tolerance          = false; // ratio-based fit, not adaptive-tolerance search
constexpr int k_control_points_auto     = 0;     // 0 = auto (ratio-driven), never a fixed override
constexpr int k_max_control_points      = 400;   // kfcompress's own default search cap
constexpr int k_skip                    = 1;     // keep every source keyframe, no decimation
constexpr bool k_force_convert_all      = false; // skip bones where conversion wouldn't shrink
                                                  // them, matching observed vanilla behavior
constexpr bool k_no_blacklist           = false; // keep the built-in weapon/camera/IK/shell-
                                                  // casing blacklist active
} // namespace

AdvancedAnimationsModule::AdvancedAnimationsModule(QWidget *parent)
    : IWindowModule(parent)
{
    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)
    layout->setContentsMargins(0, 0, 0, 0);

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // QML content is content-sized now, not full-page (see AdvancedAnimationsModule.qml /
    // GeneralBSAModule.cpp for the full rationale). Was pinned to a hardcoded opaque match for
    // QTabWidget::pane's background instead of real transparency - now that pane is translucent
    // (nebula_overrides in MainWindow.cpp, so the nebula photo behind everything actually shows),
    // this needs the same WA_AlwaysStackOnTop + transparent clear color fix as top_bar_widget_.
    qml_widget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    qml_widget_->setClearColor(Qt::transparent);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/AdvancedAnimationsModule.qml"));
    layout->addWidget(qml_widget_);
}

void AdvancedAnimationsModule::settings_to_ui(const Settings &settings)
{
    // Global override now (Profile::force_process_animations), not tied to the selected pattern -
    // see AdvancedTexturesModule for the same treatment of textures.
    bridge_.setEnabled(settings.current_profile().force_process_animations);
}

void AdvancedAnimationsModule::ui_to_settings(Settings &settings) const
{
    auto &profile = settings.current_profile();

    profile.force_process_animations = bridge_.enabled();

    // The hardcoded vanilla-matching kf::Settings values still apply per-pattern (there's only
    // ever the one base/catch-all pattern in practice, but process_file() still reads file_sets.kf
    // per matched pattern, so keep every row in sync rather than just the selected one).
    for (auto *pfs : profile.per_file_settings())
    {
        auto &kf = pfs->kf;

        kf.ratio            = k_vanilla_matching_ratio;
        kf.use_tolerance     = k_use_tolerance;
        kf.compact16          = k_compact16;
        kf.control_points     = k_control_points_auto;
        kf.max_control_points = k_max_control_points;
        kf.skip                = k_skip;
        kf.force_convert_all   = k_force_convert_all;
        kf.no_blacklist         = k_no_blacklist;
        kf.blacklist_add.clear();
    }
}

auto AdvancedAnimationsModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    return game == btu::Game::FNV;
}

auto AdvancedAnimationsModule::name() const noexcept -> QString
{
    return QObject::tr("Animations (kf compression)");
}

} // namespace cao
