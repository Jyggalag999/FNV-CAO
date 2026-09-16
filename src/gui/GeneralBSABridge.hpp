/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QObject>
#include <QString>

namespace cao {

/// @brief Thin QML-facing adapter for GeneralBSAModule. One property per widget in the old .ui,
/// same minimal split of responsibility as the other module bridges: this just holds UI state,
/// GeneralBSAModule still owns reading/writing Settings around it.
///
/// extractMode mirrors AdvancedMeshesBridge::fullOptimization: false = Create BSAs selected,
/// true = Extract BSAs selected - a plain bool is enough since exactly one is always selected
/// whenever baseChecked is true. Turning baseChecked on resets extractMode to false (Create),
/// replicating the old QGroupBox::toggled handler's "default to BSA create" behavior.
class GeneralBSABridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool baseChecked READ baseChecked WRITE setBaseChecked NOTIFY baseCheckedChanged)
    Q_PROPERTY(bool extractMode READ extractMode WRITE setExtractMode NOTIFY extractModeChanged)
    Q_PROPERTY(
        bool dontMakeLoaded READ dontMakeLoaded WRITE setDontMakeLoaded NOTIFY dontMakeLoadedChanged)
    Q_PROPERTY(bool dontRemoveFiles READ dontRemoveFiles WRITE setDontRemoveFiles NOTIFY
                   dontRemoveFilesChanged)
    Q_PROPERTY(bool dontCompress READ dontCompress WRITE setDontCompress NOTIFY dontCompressChanged)
    Q_PROPERTY(
        bool makeOverrides READ makeOverrides WRITE setMakeOverrides NOTIFY makeOverridesChanged)
    Q_PROPERTY(bool makeOverridesVisible READ makeOverridesVisible WRITE setMakeOverridesVisible
                   NOTIFY makeOverridesVisibleChanged)
    Q_PROPERTY(QString archiveName READ archiveName WRITE setArchiveName NOTIFY archiveNameChanged)
    Q_PROPERTY(bool overrideMaxSize READ overrideMaxSize WRITE setOverrideMaxSize NOTIFY
                   overrideMaxSizeChanged)
    Q_PROPERTY(int maxSizeMb READ maxSizeMb WRITE setMaxSizeMb NOTIFY maxSizeMbChanged)
    Q_PROPERTY(int defaultMaxSizeMb READ defaultMaxSizeMb WRITE setDefaultMaxSizeMb NOTIFY
                   defaultMaxSizeMbChanged)

public:
    using QObject::QObject;

    [[nodiscard]] auto baseChecked() const -> bool;
    void setBaseChecked(bool value);

    [[nodiscard]] auto extractMode() const -> bool;
    void setExtractMode(bool value);

    [[nodiscard]] auto dontMakeLoaded() const -> bool;
    void setDontMakeLoaded(bool value);

    [[nodiscard]] auto dontRemoveFiles() const -> bool;
    void setDontRemoveFiles(bool value);

    [[nodiscard]] auto dontCompress() const -> bool;
    void setDontCompress(bool value);

    [[nodiscard]] auto makeOverrides() const -> bool;
    void setMakeOverrides(bool value);

    [[nodiscard]] auto makeOverridesVisible() const -> bool;
    void setMakeOverridesVisible(bool value);

    [[nodiscard]] auto archiveName() const -> QString;
    void setArchiveName(const QString &value);

    [[nodiscard]] auto overrideMaxSize() const -> bool;
    void setOverrideMaxSize(bool value);

    [[nodiscard]] auto maxSizeMb() const -> int;
    void setMaxSizeMb(int value);

    // Informational only (not persisted to Profile) - GeneralBSAModule.cpp fills this in with
    // btu::bsa::Settings::get(target_game).max_size so the "(game default: N MB)" hint and the
    // spin box's starting value reflect the actual current-game default, not a hardcoded guess.
    [[nodiscard]] auto defaultMaxSizeMb() const -> int;
    void setDefaultMaxSizeMb(int value);

signals:
    void baseCheckedChanged();
    void extractModeChanged();
    void dontMakeLoadedChanged();
    void dontRemoveFilesChanged();
    void dontCompressChanged();
    void makeOverridesChanged();
    void makeOverridesVisibleChanged();
    void archiveNameChanged();
    void overrideMaxSizeChanged();
    void maxSizeMbChanged();
    void defaultMaxSizeMbChanged();

private:
    bool base_checked_           = false;
    bool extract_mode_           = false;
    bool dont_make_loaded_       = false;
    bool dont_remove_files_      = false;
    bool dont_compress_          = false;
    bool make_overrides_         = false;
    bool make_overrides_visible_ = true;
    QString archive_name_;
    bool override_max_size_ = false;
    int max_size_mb_        = 0;
    int default_max_size_mb_ = 0;
};

} // namespace cao
