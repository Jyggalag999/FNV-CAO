/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "main_process.hpp"

#include <btu/common/games.hpp>
#include <btu/common/metaprogramming.hpp>
#include <btu/common/string.hpp>
#include <btu/kf/compress.hpp>
#include <btu/nif/detail/common.hpp>
#include <btu/nif/mesh.hpp>
#include <btu/nif/optimize.hpp>
#include <btu/tex/optimize.hpp>
#include <btu/tex/texture.hpp>
#include <btu/tex/crunch_texture.hpp>
#include <btu/tex/crunch_functions.hpp>
#include <fmt/format.h>
#include <plog/Log.h>
#include <tl/expected.hpp>

#include <string_view>
#include <flux.hpp>
#include <flux/core/ref.hpp>
#include <flux/sequence/range.hpp>
#include <flux/adaptor/flatten_with.hpp>

template<>
struct fmt::formatter<btu::Game> : fmt::formatter<std::string_view>
{
    [[maybe_unused]] auto format(const btu::Game &game, format_context &ctx) const {
        const auto *game_str = [game] {
            switch (game)
            {
                case btu::Game::TES3: return "Morrowind";
                case btu::Game::TES4: return "Oblivion";
                case btu::Game::FNV: return "Fallout New Vegas";
                case btu::Game::SLE: return "Skyrim LE";
                case btu::Game::SSE: return "Skyrim SE";
                case btu::Game::FO4: return "Fallout 4";
                case btu::Game::Custom: return "Custom";
                default:
                {
                    assert(false && "formatter<btu::Game>: unreachable");
                    return "Invalid";
                }
            }
        }();

        return fmt::formatter<std::string_view>::format(game_str, ctx);
    }
};

template<>
struct fmt::formatter<DXGI_FORMAT> : fmt::formatter<std::string_view>
{
    [[maybe_unused]] auto format(const DXGI_FORMAT &format, format_context &ctx) const {
        return fmt::formatter<std::string_view>::format(btu::common::as_ascii(btu::tex::to_string(format)),
                                                        ctx);
    }
};

namespace cao {
auto guess_file_type(const std::filesystem::path &path) noexcept -> std::optional<FileType>
{
    const auto extension = btu::common::to_lower(path.extension().u8string());
    if (extension == u8".nif")
        return FileType::Mesh;
    if (extension == u8".dds" || extension == u8".tga")
        return FileType::Texture;
    if (extension == u8".kf")
        return FileType::Animation;

    return std::nullopt;
}

[[nodiscard]] auto steps_are_empty(const btu::nif::OptimizationSteps &steps) noexcept -> bool
{
    return !steps.format && !steps.rename_referenced_textures
           && steps.headpart == btu::nif::HeadpartStatus::No && !steps.optimize;
}

[[nodiscard]] auto steps_are_empty(const btu::tex::OptimizationSteps &steps) noexcept -> bool
{
    return !steps.convert && !steps.mipmaps && !steps.add_transparent_alpha && !steps.resize;
}

[[nodiscard]] auto human_readable_step_string(const btu::nif::OptimizationSteps &steps) noexcept -> std::string
{
    auto strs = std::vector<std::string>{};

    if (steps.format)
        strs.emplace_back(fmt::format("format: {}", steps.format.value()));
    if (steps.rename_referenced_textures)
        strs.emplace_back("rename textures tga -> dds");
    if (steps.headpart == btu::nif::HeadpartStatus::Yes)
        strs.emplace_back("processing as headpart");
    if (steps.optimize)
        strs.emplace_back("optimizing");

    return flux::from_crange(strs).flatten_with(std::string(", ")).to<std::string>();
}

[[nodiscard]] auto human_readable_step_string(const btu::tex::OptimizationSteps &steps) noexcept -> std::string
{
    auto strs = std::vector<std::string>{};

    if (steps.convert)
        strs.emplace_back(fmt::format("convert to {}", steps.best_format));
    if (steps.mipmaps)
        strs.emplace_back("generate mipmaps");
    if (steps.add_transparent_alpha)
        strs.emplace_back("add transparent alpha");
    if (steps.resize)
        strs.emplace_back(fmt::format("resize to {}x{}", steps.resize->w, steps.resize->h));

    return flux::from_crange(strs).flatten_with(std::string(", ")).to<std::string>();
}

void log_file_processing(const std::filesystem::path &path, std::string_view steps) noexcept
{
    PLOGI << fmt::format("Processing file: {}. Steps: {}", path.string(), steps);
}

void log_file_no_work_required(const std::filesystem::path &path) noexcept
{
    PLOGV << fmt::format("No work required for file: {}", path.string());
}

[[nodiscard]] auto process_mesh(btu::modmanager::ModFile &&file,
                                const btu::nif::Settings &settings,
                                const OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    return file.content
        ->and_then(
            [&file](std::vector<std::byte> content) { return btu::nif::load(file.relative_path, content); })
        .and_then([&](auto &&nif) -> tl::expected<btu::nif::Mesh, btu::common::Error> {
            auto steps = btu::nif::compute_optimization_steps(nif, settings);

            if (steps_are_empty(steps))
            {
                log_file_no_work_required(file.relative_path);
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
            }

            if (type == OptimizeType::Forced)
                steps.format = std::optional(settings.target_game); // force conversion

            log_file_processing(file.relative_path, human_readable_step_string(steps));

            if (type == OptimizeType::DryRun)
                return nif;

            return btu::nif::optimize(BTU_FWD(nif), steps);
        })
        .and_then([type](auto &&nif) -> tl::expected<std::vector<std::byte>, btu::common::Error> {
            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            return btu::nif::save(BTU_FWD(nif));
        });
}

[[nodiscard]] auto process_texture_crunch(btu::modmanager::ModFile &&file,
                                          const btu::tex::Settings &settings,
                                          const OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    return file.content
        ->and_then([&file](auto &&content) { return btu::tex::load_crunch(file.relative_path, content); })
        .and_then([&](auto &&tex) -> tl::expected<btu::tex::CrunchTexture, btu::common::Error> {
            auto steps = btu::tex::compute_optimization_steps(tex, settings);

            if (steps_are_empty(steps))
            {
                log_file_no_work_required(file.relative_path);
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
            }

            if (type == OptimizeType::Forced)
                steps.convert = true;

            // Tagged "[crunch]" specifically so it's distinguishable in the log from the DirectXTex
            // path below (process_texture) - both log the exact same step description otherwise,
            // which made it impossible to tell which encoder actually handled a given file.
            log_file_processing(file.relative_path, "[crunch] " + human_readable_step_string(steps));

            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            static auto compression_device = btu::tex::CompressionDevice();
            return btu::tex::optimize(BTU_FWD(tex), steps, compression_device);
        })
        .and_then([type](auto &&tex) -> tl::expected<std::vector<std::byte>, btu::common::Error> {
            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            return btu::tex::save(BTU_FWD(tex));
        });
}

/// crnlib (the CrunchTexture backend) can't encode every format the app might target - most notably
/// it has no BC7 encoder, which is what SSE/FO4/Starfield profiles ask for. Routing such a texture
/// through the crunch path would just fail and leave the texture completely unprocessed (see
/// crunch_supports_format's doc comment), so only take the crunch fast-path when it can actually
/// produce whatever this profile's settings might ask for.
[[nodiscard]] auto can_use_crunch(const btu::tex::Settings &settings) noexcept -> bool
{
    const auto &fmt = settings.output_format;
    return btu::tex::crunch_supports_format(fmt.uncompressed)
           && btu::tex::crunch_supports_format(fmt.uncompressed_without_alpha)
           && btu::tex::crunch_supports_format(fmt.compressed)
           && btu::tex::crunch_supports_format(fmt.compressed_without_alpha);
}

[[nodiscard]] auto process_texture(btu::modmanager::ModFile &&file,
                                   const btu::tex::Settings &settings,
                                   const OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (settings.force_crunch && can_use_crunch(settings))
        return process_texture_crunch(BTU_FWD(file), settings, type);

    static auto compression_device = btu::tex::CompressionDevice();

    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    return file.content
        ->and_then([&file](auto &&content) { return btu::tex::load(file.relative_path, content); })
        .and_then([&](auto &&tex) -> tl::expected<btu::tex::Texture, btu::common::Error> {
            auto steps = btu::tex::compute_optimization_steps(tex, settings);

            if (steps_are_empty(steps))
            {
                log_file_no_work_required(file.relative_path);
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
            }

            if (type == OptimizeType::Forced)
                steps.convert = true;

            // See the matching comment in process_texture_crunch above - tagged so the two
            // encoder paths are distinguishable in the log instead of producing identical lines.
            log_file_processing(file.relative_path, "[directxtex] " + human_readable_step_string(steps));

            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            return btu::tex::optimize(BTU_FWD(tex), steps, compression_device);
        })
        .and_then([type](auto &&tex) -> tl::expected<std::vector<std::byte>, btu::common::Error> {
            if (type == OptimizeType::DryRun)
                return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

            return btu::tex::save(BTU_FWD(tex));
        });
}

[[nodiscard]] auto process_animation(btu::modmanager::ModFile &&file,
                                     const btu::kf::Settings &kf_settings,
                                     OptimizeType type) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    if (type == OptimizeType::None)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));

    if (type == OptimizeType::DryRun)
    {
        PLOGI << std::format("{} might be optimized", file.relative_path.string());
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required));
    }

    return file.content->and_then(
        [&](std::vector<std::byte> content) { return btu::kf::compress(content, kf_settings); });
}

auto process_file(btu::modmanager::ModFile &&file, const Settings &settings) noexcept
    -> tl::expected<std::vector<std::byte>, btu::common::Error>
{
    const auto type       = guess_file_type(file.relative_path);
    const auto &file_sets = settings.current_profile().get_per_file_settings(file.relative_path);

    if (!type)
        return tl::make_unexpected(btu::common::Error(k_error_no_work_required)); // TODO: better error

    const auto get_optimize_type = [settings](OptimizeType opt_type) {
        auto dry_run         = settings.current_profile().dry_run;
        auto should_optimize = opt_type != OptimizeType::None;
        return (dry_run && should_optimize) ? OptimizeType::DryRun : opt_type;
    };

    switch (type.value())
    {
        case FileType::Mesh:
            return process_mesh(std::move(file), file_sets.nif, get_optimize_type(file_sets.nif_optimize));
        case FileType::Texture:
        {
            // Textures are driven entirely by the profile's global overrides now, not by whichever
            // pattern this file happened to match: on/compress/mipmaps/crunch/resize all ignore
            // per_file_settings_ completely (resize included, as of force_resize - it used to be
            // the one exception, gated by whichever pattern the file matched, invisibly to whatever
            // pattern happened to be selected in the GUI at the time). See
            // Profile::force_process_textures and friends (profile.hpp).
            const auto &profile = settings.current_profile();

            // "Compress uncompressed only" stands on its own: checking it is enough to compress
            // whatever's genuinely uncompressed, without also needing "Compress textures" checked.
            // It still narrows what gets touched either way - with both on, already-compressed
            // sources stay untouched same as before; this only changes what happens with
            // compress_uncompressed_only checked alone and force_compress_always off, which used
            // to silently no-op every uncompressed texture (compress() gated the whole compress
            // step, so the narrowing flag had nothing to narrow).
            auto tex_sets                       = file_sets.tex;
            tex_sets.compress = profile.force_compress_always || profile.force_compress_uncompressed_only;
            tex_sets.compress_uncompressed_only = profile.force_compress_uncompressed_only;
            tex_sets.mipmaps                    = profile.force_mipmaps_always;
            tex_sets.force_crunch               = tex_sets.force_crunch || profile.force_crunch_always;
            tex_sets.resize                     = profile.force_resize;

            // force_process_textures is the sole on/off switch. force_crunch_always only picks the
            // encoder (crnlib vs DirectXTex) for files that are already being processed - it must
            // never be able to turn processing back on by itself (that was the old LOD-override
            // bug: "Use crunch encoder" silently overrode "Process textures" being unchecked).
            const auto tex_opt_type = profile.force_process_textures ? OptimizeType::Normal : OptimizeType::None;

            return process_texture(std::move(file), tex_sets, get_optimize_type(tex_opt_type));
        }
        case FileType::Animation:
        {
            // Same story as textures: whether animations get processed is a global override, not
            // a per-pattern one. See Profile::force_process_animations (profile.hpp).
            const auto anim_opt_type = settings.current_profile().force_process_animations
                                           ? OptimizeType::Normal
                                           : OptimizeType::None;

            return process_animation(std::move(file), file_sets.kf, get_optimize_type(anim_opt_type));
        }
    }
    return tl::make_unexpected(btu::common::Error(k_unreachable));
}
} // namespace cao