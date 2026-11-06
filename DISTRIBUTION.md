# Distribution terms

blocksctl's own source files are licensed under the [ISC licence](LICENSE). Dependencies keep their original notices and licence terms. The repository does not relicense upstream files as ISC.

## Combined executable

The v0.1.0 macOS executable includes JUCE interpolation code whose original headers offer GPLv3 as an alternative to a commercial licence. This release uses the GPLv3 option and distributes the combined executable under the [GNU General Public License, version 3](LICENSES/GPL-3.0.txt).

These four files have the relevant original notices:

- `vendor/sdk/juce_audio_basics/utilities/juce_GenericInterpolator.h`
- `vendor/sdk/juce_audio_basics/utilities/juce_Interpolators.h`
- `vendor/sdk/juce_audio_basics/utilities/juce_Interpolators.cpp`
- `vendor/sdk/juce_audio_basics/utilities/juce_WindowedSincInterpolator.cpp`

They are included by the audio-basics module compiled into the tested executable. The module-level ISC declaration does not replace their file-level terms. The release audit found the interpolation lookup table in the executable, so the binary package is not described as ISC-only.

The binary archive includes GPLv3 as `LICENSE`, the project's ISC notice as `LICENSE-ISC.txt`, and the ROLI, JUCE, zlib and C++ library notices under `notices/`. These notices grant no warranty. [THIRD_PARTY.md](THIRD_PARTY.md) records the dependency revisions and source locations.

## Corresponding Source

The [v0.1.0 release](https://github.com/Xiphe/blocksctl/releases/tag/v0.1.0) offers `blocksctl-0.1.0-source.tar.gz` alongside the binary archive, with checksums for both in `SHA256SUMS`. That source archive contains the project source, pinned dependency source, retained upstream notices, CMake build configuration, workflow and test scripts. It is the Corresponding Source provided with this executable distribution.

The executable was built from revision `61f792060926d6773242bb02caf2a0adf724c2a8`. The packaging script checks that the release revision's `src/`, `vendor/` and `CMakeLists.txt` are unchanged from that build revision before it accepts the tested binary. The archive's `BUILD-INFO.json` identifies both revisions and the binary hash. Later documentation, packaging and notice changes are included in the source archive.

After extracting the source archive on a Mac with an available Apple toolchain and CMake, use:

```sh
python3 scripts/verify_vendor.py
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

macOS system frameworks and the system C++ runtime are platform dependencies, not bundled third-party binaries. Building from source requires those platform tools; running the downloaded executable does not require a compiler or a ROLI desktop application.

When redistributing the binary, retain the licence notices and provide the corresponding source as required by GPLv3. [GPLv3 sections 5 and 6](https://www.gnu.org/licenses/gpl-3.0.html) describe source and object-code distribution terms.
