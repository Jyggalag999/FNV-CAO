/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "LevelSelectorBridge.hpp"

namespace cao {

[[nodiscard]] auto get_help_text(GuiMode level) noexcept -> QString
{
    switch (level)
    {
        case GuiMode::QuickOptimize:
        {
            return QObject::tr(
                R"(QuickOptimize

                Uses default settings for porting a mod between LE and SSE.
                It will work for most of the cases and is the recommended way to port a mod.
                It is safe to apply it on a mod, and applying it to your whole mod list could help if you experience crashes.)");
        }
        case GuiMode::Medium:
        {
            return QObject::tr(
                R"(Intermediate mode

                Offers default settings for most use cases, as well as being a bit more customizable than QuickOptimize.)");
        }
        case GuiMode::Advanced:
        {
            return QObject::tr(
                R"(Advanced mode

                The full CAO experience. With profiles and patterns, you can fully customize how CAO will optimize your files.)");
        }
    }
    return QObject::tr("Unknown mode");
}

LevelSelectorBridge::LevelSelectorBridge(GuiMode initial_mode, QObject *parent)
    : QObject(parent)
    , selected_mode_(initial_mode)
    , help_text_(get_help_text(initial_mode))
{
}

auto LevelSelectorBridge::helpText() const -> QString
{
    return help_text_;
}

auto LevelSelectorBridge::rememberChoice() const -> bool
{
    return remember_choice_;
}

void LevelSelectorBridge::setRememberChoice(bool value)
{
    if (remember_choice_ == value)
        return;

    remember_choice_ = value;
    emit rememberChoiceChanged();
}

auto LevelSelectorBridge::selectedMode() const -> GuiMode
{
    return selected_mode_;
}

void LevelSelectorBridge::set_help_text(GuiMode mode)
{
    const auto text = get_help_text(mode);
    if (help_text_ == text)
        return;

    help_text_ = text;
    emit helpTextChanged();
}

void LevelSelectorBridge::select(GuiMode mode)
{
    selected_mode_ = mode;
    emit modeSelected();
}

void LevelSelectorBridge::hoverQuickOptimize()
{
    set_help_text(GuiMode::QuickOptimize);
}

void LevelSelectorBridge::hoverMedium()
{
    set_help_text(GuiMode::Medium);
}

void LevelSelectorBridge::hoverAdvanced()
{
    set_help_text(GuiMode::Advanced);
}

void LevelSelectorBridge::selectQuickOptimize()
{
    select(GuiMode::QuickOptimize);
}

void LevelSelectorBridge::selectMedium()
{
    select(GuiMode::Medium);
}

void LevelSelectorBridge::selectAdvanced()
{
    select(GuiMode::Advanced);
}

} // namespace cao
