/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "ProfilesManagerBridge.hpp"
#include "settings/base_types.hpp"

#include <btu/common/games.hpp>

#include <QDialog>

#include <array>
#include <utility>

class QQuickWidget;
class QResizeEvent;

namespace cao {
class Settings;

class ProfilesManagerWindow final : public QDialog
{
    Q_OBJECT

public:
    explicit ProfilesManagerWindow(Settings &profiles, QWidget *parent = nullptr);

    ProfilesManagerWindow(const ProfilesManagerWindow &)                     = delete;
    auto operator=(const ProfilesManagerWindow &) -> ProfilesManagerWindow & = delete;

    ProfilesManagerWindow(ProfilesManagerWindow &&)                     = delete;
    auto operator=(ProfilesManagerWindow &&) -> ProfilesManagerWindow & = delete;

    ~ProfilesManagerWindow() override = default;

    /// Display name <-> btu::Game, in combo-box order. Deliberately excludes "Fallout 3" - see
    /// ProfilesManagerBridge.hpp for why. Public so the free game_names() helper (translates this
    /// into the bridge's plain QStringList) can read it without befriending the class for it.
    static constexpr std::array<std::pair<const char *, btu::Game>, 7> k_games{{
        {"Morrowind", btu::Game::TES3},
        {"Oblivion", btu::Game::TES4},
        {"Skyrim LE (2011)", btu::Game::SLE},
        {"Skyrim SE (2016)", btu::Game::SSE},
        {"Fallout New Vegas", btu::Game::FNV},
        {"Fallout 4 NG", btu::Game::FO4},
        {"Starfield", btu::Game::Starfield},
    }};

private:
    Settings &profiles_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    ProfilesManagerBridge bridge_;
    QQuickWidget *qml_widget_ = nullptr;

    // Deliberately not layout-managed (see the constructor) - a CaoComboBox popup opening inside
    // this dialog's small fixed size (336x198) needs more room than that leaves, and MainWindow's
    // top bar hit exactly this: growing a *layout-managed* QQuickWidget to give an open popup
    // that extra room pushes every sibling below it down/forces the container to grow, instead of
    // the popup just floating over whatever's in its way like a real dropdown should. Filling the
    // dialog manually (setGeometry(), kept in sync via resizeEvent()) sidesteps that class of bug
    // entirely, the same way MainWindow's top_bar_widget_/nebula_background_widget_ do.
    [[maybe_unused]] void resizeEvent(QResizeEvent *event) override;

    void update_profiles();

    void select_profile(const QString &name);
    void select_game(int index);
    void create_profile();
    void delete_current_profile();
    void import_profile();
    void export_selected_profile();
};
} // namespace cao
