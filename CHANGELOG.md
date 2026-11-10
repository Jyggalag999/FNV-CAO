# Changelog (Fallout New Vegas fork)

## 1.2.0

- .kf animations can now be packed into BSAs (into the "- Meshes" archive).
- Fixed UI scaling: tabs scroll instead of cutting off content, and the window fits on screen.
- Fixed the progress window: the log fills the window, shows its last line and has a scrollbar.
- Opaque DXT5 textures are stored as DXT1 (lossless).
- New opt-in option: store binary-alpha diffuse DXT5 as DXT1 (1-bit alpha).
- BSA size: default 1.95 GB per archive, maximum 4044 MB.

## 1.1.0

### New
- **Error limits (per bone)** on the Animations page: maximum rotation, translation and scale
  error for compressed animations (defaults 0.25 / 0.05 / 0.001). Older profiles load with
  these defaults.
- **Remove redundant repeated keys (lossless)** checkbox on the Animations page (on by default).
- The welcome message shows the fork version.

### Changed
- Uses the kfcompress 1.6 core, so CAO and the kfcompress CLI produce the same output.
- Animation files the compressor doesn't change are no longer rewritten.
- Compressor warnings, and the reason a .kf file failed, now show in the log.
- The animation compressor is licensed GPL-2.0-only; release builds are distributed under
  GPL-2.0, and the GPL text is included.
- The README has build instructions for this fork instead of links to the upstream wiki and
  NexusMods.

## 1.0.0

First release of the Fallout New Vegas fork.
