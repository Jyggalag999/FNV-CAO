# Cathedral Assets Optimizer


**This page is for the in-development, experimental version of the tool. Documentation is likely outdated.**

# Fallout New Vegas Fork

This is a fork of Cathedral Assets Optimizer for **Fallout New Vegas only**.

- Uses CRNLib for texture compression.
- Includes a custom Animation Compressor for `.kf` files.
- Lets you fill BSAs to a configurable size limit.
- CRNLib now checks whether a texture's alpha channel is actually used, rather than only checking whether the channel exists.
- Custom UI.

This fork is released under Cathedral Assets Optimizer's original license, the [Mozilla Public License 2.0](LICENSE). The Animation Compressor included in this fork is also released by its author under the same license.

# Build instructions

Requires:

- Visual Studio 2022 with the "Desktop development with C++" workload
- [vcpkg](https://github.com/microsoft/vcpkg), cloned at `G:\CAOREPO\vcpkg`
- This repo's sibling, the [bethutil](https://github.com/Jyggalag999/bethutil) FNV fork, cloned at `G:\CAOREPO\FNV_Fork\bethutil` — the local `bethutil` vcpkg overlay port ([cmake/ports/bethutil/portfile.cmake](cmake/ports/bethutil/portfile.cmake)) points at that exact path

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
