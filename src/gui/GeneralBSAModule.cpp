/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GeneralBSAModule.hpp"

#include "utils/utils.hpp"

#include <btu/bsa/settings.hpp>
#include <btu/common/string.hpp>

#include <QColor>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace {
constexpr uint64_t k_megabyte = 1024ULL * 1024ULL;
} // namespace

namespace cao {
GeneralBSAModule::GeneralBSAModule(QWidget *parent)
    : IWindowModule(parent)
{
    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)
    layout->setContentsMargins(0, 0, 0, 0);

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // QML content is now sized to fit itself (see GeneralBSAModule.qml), not the full tab page,
    // so there's leftover space below it whenever the page is taller than the content. A
    // QQuickWidget doesn't composite with sibling widgets behind it by default - unpainted QML
    // area just shows the widget's own opaque clear color (white, unless told otherwise) - this
    // used to be pinned to a hardcoded match for QTabWidget::pane's background instead of actual
    // transparency, back when that background was itself a flat opaque color. Now that
    // MainWindow.cpp's nebula_overrides makes QTabWidget::pane translucent (so the nebula photo
    // behind everything is actually visible - see NebulaBackground.qml), a hardcoded opaque match
    // here would defeat that: this leftover space needs to be genuinely transparent, the same
    // WA_AlwaysStackOnTop + transparent clear color fix as top_bar_widget_ in MainWindow.cpp.
    qml_widget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    qml_widget_->setClearColor(Qt::transparent);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/GeneralBSAModule.qml"));
    layout->addWidget(qml_widget_);
}

void GeneralBSAModule::settings_to_ui(const Settings &settings)
{
    const auto &profile = settings.current_profile();

    switch (profile.bsa_operation)
    {
        case BsaOperation::None:
            bridge_.setBaseChecked(false);
            break;
        case BsaOperation::Create:
            bridge_.setBaseChecked(true);
            bridge_.setExtractMode(false);
            break;
        case BsaOperation::Extract:
            bridge_.setBaseChecked(true);
            bridge_.setExtractMode(true);
            break;
    }

    bridge_.setDontMakeLoaded(!profile.bsa_make_dummy_plugins);
    bridge_.setDontRemoveFiles(!profile.bsa_remove_files);
    bridge_.setDontCompress(!profile.bsa_allow_compression);

    // Only show override toggle when it makes sense.
    if (profile.target_game != btu::Game::FNV)
    {
        bridge_.setMakeOverrides(false);
        bridge_.setMakeOverridesVisible(false);
    }
    else
    {
        bridge_.setMakeOverridesVisible(true);
        bridge_.setMakeOverrides(profile.bsa_make_overrides);
    }

    bridge_.setArchiveName(to_qstring(profile.bsa_forced_name.value_or(u8"")));

    const auto default_max_size = btu::bsa::Settings::get(profile.target_game).max_size;
    bridge_.setDefaultMaxSizeMb(static_cast<int>(default_max_size / k_megabyte));

    bridge_.setOverrideMaxSize(profile.bsa_max_size.has_value());
    bridge_.setMaxSizeMb(
        static_cast<int>(profile.bsa_max_size.value_or(default_max_size) / k_megabyte));
}

void GeneralBSAModule::ui_to_settings(Settings &settings) const
{
    const bool base = bridge_.baseChecked();

    auto &profile = settings.current_profile();
    if (base && bridge_.extractMode())
        profile.bsa_operation = BsaOperation::Extract;
    else if (base && !bridge_.extractMode())
        profile.bsa_operation = BsaOperation::Create;
    else
        profile.bsa_operation = BsaOperation::None;

    profile.bsa_make_dummy_plugins = !bridge_.dontMakeLoaded();
    profile.bsa_remove_files       = !bridge_.dontRemoveFiles();
    profile.bsa_allow_compression  = !bridge_.dontCompress();
    profile.bsa_make_overrides     = bridge_.makeOverrides();

    const auto archive_name = bridge_.archiveName().trimmed().toStdString();
    profile.bsa_forced_name = archive_name.empty()
                                   ? std::nullopt
                                   : std::optional{btu::common::as_utf8_string(archive_name)};

    profile.bsa_max_size = bridge_.overrideMaxSize()
                                ? std::optional{static_cast<uint64_t>(bridge_.maxSizeMb()) * k_megabyte}
                                : std::nullopt;
}

auto GeneralBSAModule::is_supported_game(btu::Game game) const noexcept -> bool
{
    // FNV-only fork: the profile picker (ProfilesManagerWindow::k_games) never offers a
    // non-FNV game anyway, so this is purely defensive - see settings.cpp's make_base() comment.
    return game == btu::Game::FNV;
}

auto GeneralBSAModule::name() const noexcept -> QString
{
    return QObject::tr("BSA (General)");
}

} // namespace cao
