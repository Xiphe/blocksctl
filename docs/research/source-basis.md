# Source basis

Checked 6 November 2026. This note records public upstream sources for blocksctl and the limits of the initial hardware support.

## Pinned starting point

| Dependency | Revision | Reason |
| --- | --- | --- |
| [BLOCKS-SDK](https://github.com/WeAreROLI/BLOCKS-SDK) | `4582a4b4313e10be6f2d1412914ed8485fc7c010` | Complete standalone SDK with platform build examples and a bundled ISC-licensed JUCE subset. |
| [roli_blocks_basics](https://github.com/WeAreROLI/roli_blocks_basics) | `9a6df113bf570aec53d33cbf97938d8602e4ac2f` | Exact module revision pinned by that SDK's gitlink. |

The standalone SDK provides an existing dependency pair to reproduce before trying newer modules or replacing transport code. The ROLI module moved out of JUCE and renamed its namespace from `juce` to `roli`. Old code examples therefore need checking against the pinned headers. [SDK tree with gitlink](https://api.github.com/repos/WeAreROLI/BLOCKS-SDK/git/trees/4582a4b4313e10be6f2d1412914ed8485fc7c010?recursive=1), [module README](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/README.md)

The compiler, device native function table, topology discovery, remote heap synchronisation and upload logic are public source. A native host can reuse them without invoking the BLOCKS Code desktop app. [Compiler](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/littlefoot/roli_LittleFootCompiler.h), [topology interface](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/roli_PhysicalTopologySource.h), [Block interface](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/blocks/roli_Block.h), [upload implementation](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_BlockImplementation.cpp)

## Why a small native tool

The public SDK contains the parts needed to compile source and communicate with a device. Reusing those parts keeps editor project handling and UI state outside this tool's initial scope.

A search of complete reachable history in the official BLOCKS-SDK and roli_blocks_basics repositories found no complete BLOCKS Code editor application. That search included removed paths, 571 SDK commits and 93 module commits. It does not exclude private repositories, deleted repositories or unreachable source history. [SDK history](https://github.com/WeAreROLI/BLOCKS-SDK/commits/master/), [module history](https://github.com/WeAreROLI/roli_blocks_basics/commits/main/)

The official BLOCKS Code guide links to version 0.2.3. The file entered the SDK repository on 10 May 2021 with those links already present and has no subsequent change in the reachable history. This is evidence about that download page, not proof that no later application release exists. [Imported guide](https://github.com/WeAreROLI/BLOCKS-SDK/blob/aecfb72e2b5e82ce0949fad671dc7764ebbc534c/docs/getting_started_with_blocks_code.html)

## Protocol boundaries

Compilation is offline. It needs a source file, heap allocation and the native function prototypes; it does not need a connected device.

blocksctl uses the SDK's 512-byte default heap for both standalone compilation and upload. The compiler honours a source `#heapsize: N` directive. Keeping those inputs aligned avoids silently compiling a different memory layout for a device upload. [Compiler](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/littlefoot/roli_LittleFootCompiler.h), [upload implementation](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_BlockImplementation.cpp)

SDK discovery actively exchanges messages with devices and can enter API mode. A host must dispatch the event loop and report SDK lock contention. An empty device list alone cannot distinguish no device from contention or a stalled host. The official macOS BlockFinder example runs a native event loop without requiring an editor window. [Mac example](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/examples/BlockFinder/MacOS/main.mm), [detector implementation](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_MIDIDeviceDetector.cpp), [connected device group](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_ConnectedDeviceGroup.cpp)

Temporary upload and saving a default are separate operations. `upload` keeps its host session open after transfer, with Ctrl-C ending that session. `save-default` takes a source file and uploads it with the SDK's default-save persistency in the same process. The loaded callback confirms that the SDK issued the save request, not a flash acknowledgement. Device behaviour and persistence after a power cycle require hardware verification. Reading the host's current program object does not export an arbitrary program already stored on the device. [Block interface](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/blocks/roli_Block.h), [Block implementation](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_BlockImplementation.cpp)

## Verification

Native builds and eight offline tests passed on macOS 15 Apple Silicon and Ubuntu 24.04. The CI-built Mac binary also passed the offline suite on the development Mac and reproduced a known BLOCKS Code payload with matched heap settings. [Verification record](../verification/2026-11-06.md)

Apple's compiler still requires the developer to review and accept the Xcode licence before rebuilding locally on the development machine. No licence was accepted or bypassed by this project.

A [USB hardware QA pass](../verification/2026-11-06-usb-hardware.md) verified bounded discovery and explicit serial selection, temporary upload, physical light/touch/MIDI behaviour and return to the saved default after Ctrl-C on one Lightpad with firmware 1.1.0. Saving a new default and persistence after a power cycle still need a separate test. Bluetooth, disconnect handling and Linux hardware operation also remain unverified.

The initial dependency versions precede some current firmware. Matching their program format and native functions to the selected firmware remains necessary. Newer public module code is available, but updating it is a separate compatibility decision. [Current module history](https://github.com/WeAreROLI/roli_blocks_basics/commits/main/)

[THIRD_PARTY.md](../../THIRD_PARTY.md) records licence and notice requirements for the pinned source.
