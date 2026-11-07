# Cathedral Assets Optimizer


**This page is for the in-development, experimental version of the tool. Documentation is likely outdated.**

# Fallout New Vegas Fork

This is a fork of Cathedral Assets Optimizer for **Fallout New Vegas only**.

**Fork version:** 1.1.0 — see [CHANGELOG.md](CHANGELOG.md) for what's new.

- Uses CRNLib for texture compression.
- Includes a custom Animation Compressor for `.kf` files (GPL-2.0-only, see below), the same core as [kfcompress](https://github.com/Jyggalag999/FNV-AnimCompressor).
- Lets you fill BSAs to a configurable size limit.
- CRNLib now checks whether a texture's alpha channel is actually used, rather than only checking whether the channel exists.
- Custom UI.

## Animations page

- **Compress animations**: converts `.kf` keyframes to compressed B-splines.
- **Remove redundant repeated keys (lossless)**: on by default.
- **Error limits (per bone)**: the maximum rotation (degrees), translation (game units) and scale error a compressed bone may have, checked at every key and between keys. Defaults 0.25 / 0.05 / 0.001. A bone that can't stay within them is left uncompressed. Lower them for stricter output, raise them for smaller files.
- Files the compressor doesn't change are left untouched.

This fork is released under Cathedral Assets Optimizer's original license, the [Mozilla Public License 2.0](LICENSE). The Animation Compressor is GPL-2.0-only; see [License](#license) below.

# Build instructions

Requires:

- Visual Studio 2022 with the "Desktop development with C++" workload
- [vcpkg](https://github.com/microsoft/vcpkg), cloned at `P:\CAOREPO\vcpkg`
- This repo's sibling, the [bethutil](https://github.com/Jyggalag999/bethutil) FNV fork, cloned at `P:\CAOREPO\FNV_Fork\bethutil` — the local `bethutil` vcpkg overlay port ([cmake/ports/bethutil/portfile.cmake](cmake/ports/bethutil/portfile.cmake)) points at that exact path

Then, from this repo:

```powershell
cmake -S . --preset vs2022-windows
cmake --build build --config Release --target Cathedral_Assets_Optimizer
```

The built executable lands at `build\src\Release\Cathedral_Assets_Optimizer.exe`.

# Credits

A lot of people helped me throughout the development of this tool, and I would like to thank them all.
If you feel like you should be in this list, please let me know, as I'm writing this list way after the fact.

Some tools listed here might be no longer used in the project, but I still want to thank their authors for their work.

- [Zilav](https://github.com/zilav), for his assistance and [BSArch](https://github.com/TES5Edit/TES5Edit/tree/dev/Tools/BSArchive)
- [Ousnius](https://github.com/ousnius), for his assistance and [nifly](https://github.com/ousnius/nifly)
- Microsoft, for [DirectXTex](https://github.com/Microsoft/DirectXTex) (and making it Linux-compatible!)
- [Figment](figment), for [hkxcmd](https://github.com/figment/hkxcmd)
- [Deorder](https://github.com/deorder/), for his assistance and [Libbsarch](https://github.com/deorder/libbsarch)
- [Francesc M.](francescmm), for [QLogger](https://github.com/francescmm/QLogger)
- [Feles Noctis](https://ko-fi.com/felesnoctis), [Hishy](https://www.nexusmods.com/skyrimspecialedition/users/3259968), [Alsa](https://github.com/alsa64), -[Aerisarn](https://github.com/aerisarn), and many others, for tests and advice

## License

This fork's files keep their original Mozilla Public License 2.0 headers ([LICENSE](LICENSE)). The `.kf` compressor in bethutil is GPL-2.0-only ([COPYING-GPL-2.0.txt](COPYING-GPL-2.0.txt)). MPL-2.0 allows its files to be combined into a GPL-2.0 work, so built binaries of this fork that include the compressor are distributed under GPL-2.0.
