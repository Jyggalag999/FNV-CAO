/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GeneralBSAModule.hpp"

#include "utils/utils.hpp"

#include <btu/common/string.hpp>

#include <QColor>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

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
    // QQuickWidget doesn't composite with sibling widgets behind it - unpainted QML area just
    // shows the widget's own clear color, which defaults to white - so this has to be set
    // explicitly to match QTabWidget::pane's background (MainWindow.cpp's nebula_overrides)
    // rather than relying on "transparency" to reveal anything real underneath.
    qml_widget_->setClearColor(QColor("#0d0818"));
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
}

auto GeneralBSAModule::is_supported_game(btu::Game game) const noexcept -> bool
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

auto GeneralBSAModule::name() const noexcept -> QString
{
    return QObject::tr("BSA (General)");
}

} // namespace cao
