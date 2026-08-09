/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "AdvancedTexturesBridge.hpp"

#include <algorithm>

namespace cao {

auto AdvancedTexturesBridge::mainChecked() const -> bool
{
    return main_checked_;
}

void AdvancedTexturesBridge::setMainChecked(bool value)
{
    if (main_checked_ == value)
        return;

    main_checked_ = value;
    emit mainCheckedChanged();
}

auto AdvancedTexturesBridge::compress() const -> bool
{
    return compress_;
}

void AdvancedTexturesBridge::setCompress(bool value)
{
    if (compress_ == value)
        return;

    compress_ = value;
    emit compressChanged();
}

auto AdvancedTexturesBridge::mipmaps() const -> bool
{
    return mipmaps_;
}

void AdvancedTexturesBridge::setMipmaps(bool value)
{
    if (mipmaps_ == value)
        return;

    mipmaps_ = value;
    emit mipmapsChanged();
}

auto AdvancedTexturesBridge::forceCrunch() const -> bool
{
    return force_crunch_;
}

void AdvancedTexturesBridge::setForceCrunch(bool value)
{
    if (force_crunch_ == value)
        return;

    force_crunch_ = value;
    emit forceCrunchChanged();
}

auto AdvancedTexturesBridge::resizingChecked() const -> bool
{
    return resizing_checked_;
}

void AdvancedTexturesBridge::setResizingChecked(bool value)
{
    if (resizing_checked_ == value)
        return;

    resizing_checked_ = value;
    emit resizingCheckedChanged();
}

auto AdvancedTexturesBridge::resizeByRatio() const -> bool
{
    return resize_by_ratio_;
}

void AdvancedTexturesBridge::setResizeByRatio(bool value)
{
    if (resize_by_ratio_ == value)
        return;

    resize_by_ratio_ = value;
    emit resizeByRatioChanged();

    // Disable minimum size if not by ratio (mirrors the old update_min_size() handler).
    if (!value)
        setMinimumChecked(false);
}

auto AdvancedTexturesBridge::width() const -> int
{
    return width_;
}

void AdvancedTexturesBridge::setWidth(int value)
{
    if (width_ == value)
        return;

    width_ = value;
    emit widthChanged();

    if (resize_by_ratio_)
        setHeight(value);
}

auto AdvancedTexturesBridge::height() const -> int
{
    return height_;
}

void AdvancedTexturesBridge::setHeight(int value)
{
    if (height_ == value)
        return;

    height_ = value;
    emit heightChanged();

    if (resize_by_ratio_)
        setWidth(value);
}

auto AdvancedTexturesBridge::minimumChecked() const -> bool
{
    return minimum_checked_;
}

void AdvancedTexturesBridge::setMinimumChecked(bool value)
{
    if (minimum_checked_ == value)
        return;

    minimum_checked_ = value;
    emit minimumCheckedChanged();
}

auto AdvancedTexturesBridge::minimumWidth() const -> int
{
    return minimum_width_;
}

void AdvancedTexturesBridge::setMinimumWidth(int value)
{
    if (minimum_width_ == value)
        return;

    minimum_width_ = value;
    emit minimumWidthChanged();
}

auto AdvancedTexturesBridge::minimumHeight() const -> int
{
    return minimum_height_;
}

void AdvancedTexturesBridge::setMinimumHeight(int value)
{
    if (minimum_height_ == value)
        return;

    minimum_height_ = value;
    emit minimumHeightChanged();
}

int AdvancedTexturesBridge::roundUpPow2(int value) const
{
    return static_cast<int>(btu::tex::util::upper_pow2(static_cast<size_t>(std::max(value, 0))));
}

} // namespace cao
