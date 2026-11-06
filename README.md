# blocksctl

Compile Littlefoot programs and load them onto ROLI BLOCKS from a terminal. The first target is the Lightpad BLOCK.

blocksctl reuses ROLI's public compiler and device SDK. Its native implementation is intended to run directly against the device, without BLOCKS Code, ROLI Dashboard, ROLI Connect, an account, or a ROLI server at runtime. The pinned dependency sources are included in this repository.

## Status

This is an initial source implementation. A native build has not been verified because the development machine's Xcode licence has not been accepted. Hardware discovery, upload and persistence have not been validated. Review and accept the Xcode licence yourself before building with Apple's toolchain.

The published SDK provides the compiler and upload APIs. Their presence does not establish that this implementation works with a particular device firmware. [Source basis](docs/research/source-basis.md) records the pinned revisions and remaining verification work.

## Build

The first target is macOS, with an accepted Xcode toolchain licence, a C++17 compiler, CMake and Python 3 for the smoke test. Linux build support is also intended. On Ubuntu, install `libasound2-dev`, `libx11-dev` and `pkg-config` alongside the compiler and build tools. Linux hardware support remains unverified.

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The executable is `build/blocksctl`. The smoke test covers local compilation and command validation. It does not contact hardware.

## Commands

```sh
./build/blocksctl compile program.littlefoot -o program.bin
./build/blocksctl compile program.littlefoot -o program.h --format header
./build/blocksctl devices --timeout 5
./build/blocksctl upload program.littlefoot --device SERIAL --timeout 15
./build/blocksctl save-default program.littlefoot --device SERIAL --timeout 15
```

| Command | Effect |
| --- | --- |
| `compile SOURCE -o OUTPUT` | Compiles a local source file. It never discovers or contacts devices. The default format is `binary`; `header` writes a C++ byte array. |
| `devices` | Actively discovers BLOCKS devices. This sends SDK protocol messages and can enter device API mode. It is an explicit device operation. |
| `upload SOURCE --device SERIAL` | Compiles and uploads a temporary program to the selected device, then keeps the host session open until Ctrl-C. It does not save a new startup default. |
| `save-default SOURCE --device SERIAL` | Compiles and uploads the source as a startup default, requests a save, then exits after a short settling period. |

Device commands accept `--timeout SECONDS`. For `upload`, this bounds discovery and transfer; it does not end the session after a successful transfer. Use the serial reported by discovery to select a device. Close other SDK hosts before discovery if they hold the SDK's device lock.

Compilation and upload both use a 512-byte default heap. A `#heapsize: N` directive in the source overrides it. There is no separate heap-size command option.

Temporary upload and saving the default are separate actions. `save-default` requires the source so that uploading and requesting the save happen in one host session. An upload completion callback reports transfer state; it does not prove that the device runs the expected behaviour. The SDK provides no flash-save acknowledgement here, so a save request does not prove persistence across a power cycle.

## Scope

The initial workflow uses plain `.littlefoot` source files. Editor projects, Dashboard parameter metadata, image import, firmware updates and exporting an unknown program already on a device are outside the initial scope. Other BLOCKS models and Bluetooth require separate verification.

## Licence

blocksctl's own code is [ISC licensed](LICENSE). Upstream code keeps its own notices. [THIRD_PARTY.md](THIRD_PARTY.md) records the SDK dependencies and their pinned licences.
