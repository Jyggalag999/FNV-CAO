/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <btu/tex/dimension.hpp>

#include <QObject>

namespace cao {

/// @brief Thin QML-facing adapter for AdvancedTexturesModule. One property per widget in the old
/// .ui, same minimal split of responsibility as the other module bridges - this just holds UI
/// state, AdvancedTexturesModule still owns reading/writing Settings around it.
///
/// Two behaviors from the old widget-signal wiring are replicated here instead of in QML, same
/// as AdvancedMeshesBridge/GeneralBSABridge's "reset on enable" logic:
///  - In ratio mode (resizeByRatio), width and height are the same "ratio" number and stay in
///    sync live - setWidth/setHeight each push their new value into the other.
///  - Leaving ratio mode force-unchecks minimumChecked, mirroring the old
///    "disable minimum size if not by ratio" handler.
/// roundUpPow2() wraps btu::tex::util::upper_pow2 (the processing backend, untouched) for QML to
/// call from each size field's onEditingFinished, replicating the old per-spin-box
/// editingFinished handlers without exposing backend internals to QML directly.
class AdvancedTexturesBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool mainChecked READ mainChecked WRITE setMainChecked NOTIFY mainCheckedChanged)
    Q_PROPERTY(bool compress READ compress WRITE setCompress NOTIFY compressChanged)
    Q_PROPERTY(bool mipmaps READ mipmaps WRITE setMipmaps NOTIFY mipmapsChanged)
    Q_PROPERTY(bool forceCrunch READ forceCrunch WRITE setForceCrunch NOTIFY forceCrunchChanged)

    Q_PROPERTY(
        bool resizingChecked READ resizingChecked WRITE setResizingChecked NOTIFY resizingCheckedChanged)
    Q_PROPERTY(
        bool resizeByRatio READ resizeByRatio WRITE setResizeByRatio NOTIFY resizeByRatioChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(
        bool minimumChecked READ minimumChecked WRITE setMinimumChecked NOTIFY minimumCheckedChanged)
    Q_PROPERTY(int minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)
    Q_PROPERTY(
        int minimumHeight READ minimumHeight WRITE setMinimumHeight NOTIFY minimumHeightChanged)

public:
    using QObject::QObject;

    [[nodiscard]] auto mainChecked() const -> bool;
    void setMainChecked(bool value);

    [[nodiscard]] auto compress() const -> bool;
    void setCompress(bool value);

    [[nodiscard]] auto mipmaps() const -> bool;
    void setMipmaps(bool value);

    [[nodiscard]] auto forceCrunch() const -> bool;
    void setForceCrunch(bool value);

    [[nodiscard]] auto resizingChecked() const -> bool;
    void setResizingChecked(bool value);

    [[nodiscard]] auto resizeByRatio() const -> bool;
    void setResizeByRatio(bool value);

    [[nodiscard]] auto width() const -> int;
    void setWidth(int value);

    [[nodiscard]] auto height() const -> int;
    void setHeight(int value);

    [[nodiscard]] auto minimumChecked() const -> bool;
    void setMinimumChecked(bool value);

    [[nodiscard]] auto minimumWidth() const -> int;
    void setMinimumWidth(int value);

    [[nodiscard]] auto minimumHeight() const -> int;
    void setMinimumHeight(int value);

    [[nodiscard]] Q_INVOKABLE int roundUpPow2(int value) const;

signals:
    void mainCheckedChanged();
    void compressChanged();
    void mipmapsChanged();
    void forceCrunchChanged();
    void resizingCheckedChanged();
    void resizeByRatioChanged();
    void widthChanged();
    void heightChanged();
    void minimumCheckedChanged();
    void minimumWidthChanged();
    void minimumHeightChanged();

private:
    bool main_checked_     = false;
    bool compress_         = false;
    bool mipmaps_          = false;
    bool force_crunch_     = false;
    bool resizing_checked_ = false;
    bool resize_by_ratio_  = true;
    int width_             = 2;
    int height_            = 2;
    bool minimum_checked_  = false;
    int minimum_width_     = 4;
    int minimum_height_    = 4;
};

} // namespace cao
