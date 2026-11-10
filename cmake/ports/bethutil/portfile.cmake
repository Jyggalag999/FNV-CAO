# cache-bust: bump this when SOURCE_PATH content changes, since vcpkg can't hash a local overlay path (bump 22: FNV packs .kf animations into the meshes BSA; earlier bump 21: FNV BSA default max size 1.95 GiB (override up to 3.95 GiB); earlier bump 20: binary-alpha diffuse DXT5 stored as DXT1 with 1-bit alpha (opt-in); earlier bump 19: opaque DXT5 stored as DXT1 via lossless block copy; earlier bump 18: constant channels stored as static pose, clips under 4 frames skipped, converted bones' old key-data blocks removed; earlier bump 17: engine-exact between-key reference (Bezier/Hermite/step keys, Euler per-axis, engine slerp/squad); earlier bump 16: per-bone error-budget fit with engine-exact verification, sequence-range interpolators, 0xFFFF handle padding; earlier bump 15: no control-point cap by default, sparse basis rows; earlier bump 14: keep original when B-spline output is not smaller; earlier bump 13: merged review fixes (parser count checks, FormatError, core cleanup); earlier bump 12: kf adapter maps FormatError/bad_alloc/other, exposes modified; earlier bump 11: NIF version error prints real hex; earlier bump 10: shared kfcompress v1.5 core (dedupe, fit-against-original-keys, min control points), no temp files; earlier bump 9: btu::kf bspline solve adds a second-derivative roughness penalty to fix Runge's-phenomenon control-point swings on short/sparse clips, ported from kfcompress-beta-workspace)
set(SOURCE_PATH "P:/CAOREPO/FNV_Fork/bethutil")

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS
	-DBUILD_TESTING=OFF
)
vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/bethutil")

file(REMOVE_RECURSE
	${CURRENT_PACKAGES_DIR}/debug/include
	${CURRENT_PACKAGES_DIR}/debug/share
)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)