# blocksctl

Compile Littlefoot programs and load them onto ROLI BLOCKS from a terminal. The first target is the Lightpad BLOCK.

blocksctl reuses ROLI's public compiler and device SDK. It runs directly against the device, without BLOCKS Code, ROLI Dashboard, ROLI Connect, an account, or a ROLI server at runtime. The pinned dependency sources are included in this repository.

## Install on macOS

Download [blocksctl 0.1.0 for Apple Silicon](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/blocksctl-0.1.0-macos-arm64.tar.gz) and [SHA256SUMS](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/SHA256SUMS), then follow the [checksum and user-local installation steps](docs/install-macos.md). The binary requires macOS 15.0 or later and an Apple Silicon Mac. It does not require Xcode or ROLI software.

The first release is ad-hoc signed, without Developer ID signing or Apple notarization. The install guide covers macOS's per-item approval. Intel Macs need a source build; no Intel Mac binary is included. [Release notes](docs/releases/0.1.0.md)

The combined binary is distributed under GPLv3. Its [complete Corresponding Source](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/blocksctl-0.1.0-source.tar.gz) is available on the same release. blocksctl's own source remains ISC licensed. [Distribution terms](DISTRIBUTION.md)

## Status

Native builds and eight offline tests pass on macOS 15 Apple Silicon and Ubuntu 24.04. The CI-built Mac executable also passes the tests on the development Mac. With a matching heap directive, it reproduces a known BLOCKS Code payload byte for byte. [Verification record](docs/verification/2026-11-06.md)

USB discovery, temporary upload, physical light/touch/MIDI interaction, clean Ctrl-C shutdown and return to the saved default passed on one Lightpad with firmware 1.1.0. BLOCKS Code and Dashboard were closed. [USB hardware QA](docs/verification/2026-11-06-usb-hardware.md)

A further pass verified Bluetooth-only discovery and temporary upload, USB and Bluetooth disconnect/reconnect recovery, and `save-default` with a full power cycle. The saved baseline was then restored and checked after a second power cycle. [Bluetooth, recovery and persistence QA](docs/verification/2026-11-06-device-lifecycle.md)

Hardware verification covers one Lightpad on macOS. Linux hardware operation, disconnects during transfer and long-duration reliability remain unverified. Local rebuilding on the development Mac remains blocked by its unaccepted Xcode licence; the CI-built executable runs without a local compiler.

[Source basis](docs/research/source-basis.md) records the pinned SDK revisions and verification boundaries.

## Build

The first target is macOS, with an accepted Xcode toolchain licence, a C++17 compiler, CMake and Python 3 for the smoke test. Linux builds are checked on Ubuntu 24.04. On Ubuntu, install `libasound2-dev` and `pkg-config` alongside the compiler and build tools. Linux hardware support remains unverified.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The executable is `build/blocksctl`. The smoke test covers local compilation and command validation. It does not contact hardware.

[GitHub Actions](https://github.com/Xiphe/blocksctl/actions/workflows/build.yml) builds executables and attaches development artifacts to successful runs. The [v0.1.0 release](https://github.com/Xiphe/blocksctl/releases/tag/v0.1.0) packages the exact Mac executable used in the recorded hardware checks. Dependency file hashes can be checked with `python3 scripts/verify_vendor.py`.

## Commands

```sh
blocksctl compile program.littlefoot -o program.bin
blocksctl compile program.littlefoot -o program.h --format header
blocksctl devices --timeout 5
blocksctl upload program.littlefoot --device SERIAL --timeout 15
blocksctl save-default program.littlefoot --device SERIAL --timeout 15
```

| Command | Effect |
| --- | --- |
| `compile SOURCE -o OUTPUT` | Compiles a local source file. It never discovers or contacts devices. The default format is `binary`; `header` writes a C++ byte array. |
| `devices` | Actively discovers BLOCKS devices. This sends SDK protocol messages and can enter device API mode. It is an explicit device operation. |
| `upload SOURCE --device SERIAL` | Compiles and uploads a temporary program to the selected device, then keeps the host session open until Ctrl-C. It does not save a new startup default. |
| `save-default SOURCE --device SERIAL` | Compiles and uploads the source as a startup default, requests a save, then exits after a short settling period. |

Device commands accept `--timeout SECONDS`. For `upload`, this bounds discovery and transfer; it does not end the session after a successful transfer. Use the serial reported by discovery to select a device. Close other SDK hosts before discovery if they hold the SDK's device lock.

Compilation and upload both use a 512-byte default heap. A `#heapsize: N` directive in the source overrides it. There is no separate heap-size command option.

Temporary upload and saving the default are separate actions. `save-default` requires the source so that uploading and requesting the save happen in one host session. It overwrites the saved startup program. Keep the source of the previous default if you need to restore it, then verify a new default after fully powering the device off and on. The SDK supplies no flash-save acknowledgement; command success alone does not prove persistence.

## Scope

The initial workflow uses plain `.littlefoot` source files. Editor projects, Dashboard parameter metadata, image import, firmware updates and exporting an unknown program already on a device are outside the initial scope. Other BLOCKS models, firmware versions and operating systems need separate hardware verification.

## Licence

blocksctl's own code is [ISC licensed](LICENSE). The v0.1.0 combined binary contains GPLv3 JUCE code and is distributed under GPLv3, with complete Corresponding Source available beside the binary archive. [DISTRIBUTION.md](DISTRIBUTION.md) explains the binary terms. Upstream code retains its notices; [THIRD_PARTY.md](THIRD_PARTY.md) records dependency licences.
