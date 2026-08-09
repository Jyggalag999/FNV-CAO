/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ProfilesManagerWindow.hpp"

#include "settings/json.hpp"
#include "settings/settings.hpp"
#include "utils/utils.hpp"

#include <flux.hpp>

#include <algorithm>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {

namespace {
[[nodiscard]] auto game_names() -> QStringList
{
    QStringList names;
    for (const auto &[name, game] : ProfilesManagerWindow::k_games)
        names << name;
    return names;
}
} // namespace

ProfilesManagerWindow::ProfilesManagerWindow(Settings &profiles, QWidget *parent)
    : QDialog(parent)
    , profiles_(profiles)
    , bridge_(game_names())
{
    setWindowTitle(tr("Profiles manager"));
    resize(336, 198);

    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_widget_->rootContext()->setContextProperty("bridge", &bridge_);
    qml_widget_->setSource(QUrl("qrc:/qml/ProfilesManagerWindow.qml"));
    layout->addWidget(qml_widget_);

    connect(&bridge_, &ProfilesManagerBridge::selectProfileRequested, this,
            &ProfilesManagerWindow::select_profile);
    connect(&bridge_, &ProfilesManagerBridge::selectGameRequested, this,
            &ProfilesManagerWindow::select_game);
    connect(&bridge_, &ProfilesManagerBridge::newProfileRequested, this,
            &ProfilesManagerWindow::create_profile);
    connect(&bridge_, &ProfilesManagerBridge::removeProfileRequested, this,
            &ProfilesManagerWindow::delete_current_profile);
    connect(&bridge_, &ProfilesManagerBridge::importProfileRequested, this,
            &ProfilesManagerWindow::import_profile);
    connect(&bridge_, &ProfilesManagerBridge::exportProfileRequested, this,
            &ProfilesManagerWindow::export_selected_profile);

    update_profiles();
}

void ProfilesManagerWindow::update_profiles()
{
    const auto names = flux::from(profiles_.list_profiles())
                            .map([](const auto &p) { return QString::fromUtf8(p.data(), p.size()); })
                            .to<QList>();
    bridge_.setProfileNames(names);
    bridge_.setCurrentProfile(to_qstring(profiles_.current_profile_name()));

    const auto game_it = std::ranges::find(k_games, profiles_.current_profile().target_game,
                                           &std::pair<const char *, btu::Game>::second);
    bridge_.setCurrentGameIndex(game_it != k_games.end()
                                     ? static_cast<int>(std::distance(k_games.begin(), game_it))
                                     : -1);
}

void ProfilesManagerWindow::select_profile(const QString &name)
{
    const bool success = profiles_.set_current_profile(to_u8string(name));
    assert(success);
    (void)success;

    update_profiles();
}

void ProfilesManagerWindow::select_game(int index)
{
    if (index < 0 || static_cast<size_t>(index) >= k_games.size())
        return;

    profiles_.current_profile().target_game = k_games[static_cast<size_t>(index)].second;
    bridge_.setCurrentGameIndex(index);
}

void ProfilesManagerWindow::create_profile()
{
    bool ok = false;
    const QString &text
        = QInputDialog::getText(this, tr("New profile"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    // Choosing base profile

    const auto &profiles_list = bridge_.profileNames();
    const int current_index   = static_cast<int>(profiles_list.indexOf(bridge_.currentProfile()));

    bool item_ok               = false;
    const QString base_profile_text
        = QInputDialog::getItem(this,
                                tr("Base profile"),
                                tr("Which profile do you want to use as a base?"),
                                profiles_list,
                                std::max(current_index, 0),
                                false,
                                &item_ok);

    if (!item_ok)
        return;

    // should be safe to dereference, since we just checked that it exists
    auto base_profile = profiles_.get_profile(to_u8string(base_profile_text)).value();

    profiles_.create_profile(to_u8string(text), std::move(base_profile));
    const bool success = profiles_.set_current_profile(to_u8string(text));
    assert(success);
    (void)success;

    update_profiles();
}

void ProfilesManagerWindow::delete_current_profile()
{
    const QString &current = bridge_.currentProfile();
    const auto button      = QMessageBox::warning(
        this,
        tr("Remove profile"),
        tr("Are you sure you want to remove profile '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    profiles_.remove(to_u8string(current));
    update_profiles();
}

void ProfilesManagerWindow::import_profile()
{
    const auto raw_path = QFileDialog::getOpenFileName(this,
                                                       tr("Import profile"),
                                                       QString(),
                                                       tr("Profiles (*.json)"));
    if (raw_path.isEmpty())
        return;

    const auto path = btu::Path(to_u8string(raw_path));

    auto profile = json::read_from_file<Profile>(path);
    if (!profile)
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load profile"));
        return;
    }

    const auto name = path.filename().u8string();
    profiles_.create_profile(name, *std::move(profile));
    update_profiles();
}

void ProfilesManagerWindow::export_selected_profile()
{
    const auto path = QFileDialog::getSaveFileName(this,
                                                   tr("Export profile"),
                                                   QString(),
                                                   tr("Profiles (*.json)"));
    if (path.isEmpty())
        return;

    const auto profile = profiles_.get_profile(to_u8string(bridge_.currentProfile())).value();

    if (!json::save_to_file(profile, to_u8string(path)))
        QMessageBox::critical(this, tr("Error"), tr("Failed to save profile"));
}

} // namespace cao
