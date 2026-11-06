# Distribution

| Material | Terms |
| --- | --- |
| blocksctl-authored source | [ISC](LICENSE) |
| v0.1.0 combined executable | [GPLv3](LICENSES/GPL-3.0.txt) |
| Vendored source | Original file-level notices; [dependency inventory](THIRD_PARTY.md) |

## Binary licence

The tested binary retains code from [four JUCE interpolator files](THIRD_PARTY.md#file-level-terms) offering GPLv3 or commercial terms. This distribution selects GPLv3. Module-level ISC declarations do not override their headers.

## Release contents

| Asset | Contents |
| --- | --- |
| `blocksctl-0.1.0-macos-arm64.tar.gz` | Executable, install instructions, notices, `BUILD-INFO.json` |
| `blocksctl-0.1.0-source.tar.gz` | Complete Corresponding Source, pinned dependencies, notices, build and test scripts |
| `SHA256SUMS` | Both archive checksums |

All three are on the [same release](https://github.com/Xiphe/blocksctl/releases/tag/v0.1.0).

The binary package's `LICENSE` is GPLv3; `LICENSE-ISC.txt` covers our code. `notices/` contains ROLI, JUCE, zlib and C++ library notices. macOS frameworks and the system C++ runtime are external platform dependencies.

## Provenance

- Executable build: `61f792060926d6773242bb02caf2a0adf724c2a8`.
- Packaging requires identical `src/`, `vendor/` and `CMakeLists.txt` at the release revision.
- `BUILD-INFO.json` records both revisions and the binary hash.
- [Build from source](docs/build.md); [GPLv3 distribution requirements, sections 5 and 6](https://www.gnu.org/licenses/gpl-3.0.html).
