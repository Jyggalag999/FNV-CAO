/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

namespace cao {

/// @brief Thin QML-facing adapter for ProfilesManagerWindow. btu::Game stays out of the bridge
/// entirely (same reasoning as LevelSelectorBridge/PatternsManagerModel keeping their respective
/// enums out of QML) - gameNames is just display strings, supplied by ProfilesManagerWindow at
/// construction (it owns the actual name <-> btu::Game mapping), and currentGameIndex/
/// requestSelectGame() deal only in plain indices into that list.
///
/// Deliberately drops "Fallout 3" from the old Widgets version's game combo box: that item had no
/// corresponding set_data() call (btu::Game has no FO3 value at all), so selecting it silently
/// left target_game holding whatever an empty QVariant.value<btu::Game>() default-constructs to -
/// a real trap, not a feature worth preserving. See ProfilesManagerWindow's commit for more.
class ProfilesManagerBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList profileNames READ profileNames NOTIFY profileNamesChanged)
    Q_PROPERTY(QString currentProfile READ currentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(QStringList gameNames READ gameNames CONSTANT)
    Q_PROPERTY(int currentGameIndex READ currentGameIndex NOTIFY currentGameIndexChanged)

public:
    explicit ProfilesManagerBridge(QStringList game_names, QObject *parent = nullptr);

    [[nodiscard]] auto profileNames() const -> QStringList;
    void setProfileNames(QStringList names);

    [[nodiscard]] auto currentProfile() const -> QString;
    void setCurrentProfile(const QString &name);

    [[nodiscard]] auto gameNames() const -> QStringList;

    [[nodiscard]] auto currentGameIndex() const -> int;
    void setCurrentGameIndex(int index);

    Q_INVOKABLE void requestSelectProfile(const QString &name);
    Q_INVOKABLE void requestSelectGame(int index);
    Q_INVOKABLE void requestNewProfile();
    Q_INVOKABLE void requestRemoveProfile();
    Q_INVOKABLE void requestImportProfile();
    Q_INVOKABLE void requestExportProfile();

signals:
    void profileNamesChanged();
    void currentProfileChanged();
    void currentGameIndexChanged();

    void selectProfileRequested(QString name);
    void selectGameRequested(int index);
    void newProfileRequested();
    void removeProfileRequested();
    void importProfileRequested();
    void exportProfileRequested();

private:
    QStringList profile_names_;
    QString current_profile_;
    QStringList game_names_;
    int current_game_index_ = 0;
};

} // namespace cao
