/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "base_types.hpp"
#include "per_file_settings.hpp"

#include <btu/common/games.hpp>
#include <btu/common/path.hpp>
#include <nlohmann/json.hpp>

#include <optional>
#include <variant>

namespace cao {
enum class BsaOperation : std::uint8_t
{
    None,
    Create,
    Extract,
};

class Profile
{
public:
    BsaOperation bsa_operation  = BsaOperation::None;
    bool bsa_make_dummy_plugins = true;
    bool bsa_remove_files       = true;
    bool bsa_allow_compression  = true;
    bool bsa_make_overrides     = false;

    // User-typed base name for the output BSA(s)/ESP. When set, overrides the auto-detected
    // plugin/folder name for every archive type, and only one dummy ESP gets created total.
    std::optional<std::u8string> bsa_forced_name;

    // Overrides btu::bsa::Settings::get(target_game)'s max_size (in bytes) when set. Lets the max
    // size to pack a BSA up to be changed from the GUI/profile instead of needing a source edit +
    // rebuild every time (see manager.cpp's get_bsa_settings()).
    std::optional<uint64_t> bsa_max_size;

    bool dry_run = false;

    // Global overrides for textures/animations/BSA-packing. The per-pattern list
    // (per_file_settings_) still exists, but for the fields below it's no longer consulted at all
    // (resize included, as of force_resize below): these flags and force_resize are the sole
    // authority for whether/how textures, animations, and packing behave, regardless of which
    // pattern a file matches or which pattern happens to be selected in the GUI. Replaces the
    // older per-pattern tex_optimize/tex.compress/tex.mipmaps/tex.resize/anim_optimize/pack fields
    // plus the old LOD-only force_lod_compress_crunch carve-out. Packing keeps bethutil's own
    // structural gate (is_legal_path in pack.cpp: files at the archive root, non-regular files)
    // untouched - this only replaces the user-configurable per-pattern "pack: bool" layer on top
    // of that.
    bool force_process_textures            = true;
    bool force_compress_always             = false;
    bool force_compress_uncompressed_only  = false; // only touch textures not already compressed
    bool force_mipmaps_always              = true;
    bool force_process_animations          = true;
    bool force_pack_always                 = true;

    bool force_crunch_always = false;

    // Global override for resize: fully replaces whatever any per_file_settings_ pattern would
    // have asked for, same as the flags above. std::monostate means "never resize" - resize only
    // ever happens when this is explicitly set to a Dimension or ResizeRatio (i.e. the GUI's
    // "Resizing" checkbox is on), never implicitly from a pattern match.
    std::variant<std::monostate, btu::tex::util::ResizeRatio, btu::tex::Dimension> force_resize;

    uint32_t gpu_index{0};

    OptimizationMode optimization_mode = OptimizationMode::SingleMod;
    btu::Game target_game              = btu::Game::SSE;

    btu::Path input_path;

    std::vector<std::u8string> mods_blacklist;

    [[nodiscard]] auto per_file_settings() noexcept -> std::vector<PerFileSettings *>
    {
        std::vector<PerFileSettings *> result;
        for (auto &settings : per_file_settings_)
            result.push_back(&settings);

        result.push_back(&base_per_file_settings_);

        return result;
    }

    [[nodiscard]] auto per_file_settings() const noexcept -> std::vector<const PerFileSettings *>
    {
        std::vector<const PerFileSettings *> result;
        for (const auto &settings : per_file_settings_)
            result.push_back(&settings);

        result.push_back(&base_per_file_settings_);

        return result;
    }

    void prepend_per_file_settings(PerFileSettings settings) noexcept
    {
        per_file_settings_.insert(per_file_settings_.begin(), std::move(settings));
    }

    void append_per_file_settings(PerFileSettings settings) noexcept
    {
        per_file_settings_.emplace_back(std::move(settings));
    }

    auto remove_per_file_settings(std::u8string_view text) noexcept -> bool
    {
        const auto it = std::ranges::find(per_file_settings_, text, [](const auto &pfs) {
            return pfs.pattern.text();
        });

        if (it == per_file_settings_.end())
            return false;

        per_file_settings_.erase(it);
        return true;
    }

    void move_per_file_settings(const int original_index, const int new_index) noexcept
    {
        if (original_index == new_index)
            return;

        auto target = new_index;
        if (original_index < new_index)
            target--;

        const auto pfs = per_file_settings_[original_index];

        per_file_settings_.erase(per_file_settings_.begin() + original_index);
        per_file_settings_.insert(per_file_settings_.begin() + target, std::move(pfs));
    }

    [[nodiscard]] auto get_per_file_settings(const std::filesystem::path &path) const noexcept
        -> PerFileSettings
    {
        const auto it = std::ranges::find_if(per_file_settings_, [&path](const auto &settings) {
            return settings.matches(path);
        });

        if (it == per_file_settings_.end())
            return base_per_file_settings_;

        return *it;
    }

    [[nodiscard]] static auto make_base(btu::Game game) noexcept -> Profile;

private:
    PerFileSettings base_per_file_settings_;
    std::vector<PerFileSettings> per_file_settings_;

    [[nodiscard]] static auto set_fnv_settings(Profile profile) noexcept -> Profile;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Profile,
                                   bsa_operation,
                                   bsa_make_dummy_plugins,
                                   bsa_allow_compression,
                                   bsa_make_overrides,
                                   bsa_forced_name,
                                   bsa_max_size,
                                   dry_run,
                                   force_process_textures,
                                   force_compress_always,
                                   force_compress_uncompressed_only,
                                   force_mipmaps_always,
                                   force_process_animations,
                                   force_pack_always,
                                   force_crunch_always,
                                   force_resize,
                                   gpu_index,
                                   optimization_mode,
                                   target_game,
                                   input_path,
                                   mods_blacklist,
                                   base_per_file_settings_,
                                   per_file_settings_)
};

NLOHMANN_JSON_SERIALIZE_ENUM(BsaOperation,
                             {{BsaOperation::None, nullptr},
                              {BsaOperation::Create, "create"},
                              {BsaOperation::Extract, "extract"}})

} // namespace cao