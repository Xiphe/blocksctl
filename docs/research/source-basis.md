# Source basis

Checked 2026-11-06.

| Dependency | Pinned revision |
| --- | --- |
| [BLOCKS-SDK](https://github.com/WeAreROLI/BLOCKS-SDK/tree/4582a4b4313e10be6f2d1412914ed8485fc7c010) | `4582a4b4313e10be6f2d1412914ed8485fc7c010` |
| [roli_blocks_basics](https://github.com/WeAreROLI/roli_blocks_basics/tree/9a6df113bf570aec53d33cbf97938d8602e4ac2f) | `9a6df113bf570aec53d33cbf97938d8602e4ac2f`, SDK gitlink |

Standalone SDK dependency pair retained together. Module namespace is `roli`; older `juce::` examples require adaptation. [Gitlink](https://api.github.com/repos/WeAreROLI/BLOCKS-SDK/git/trees/4582a4b4313e10be6f2d1412914ed8485fc7c010?recursive=1), [module README](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/README.md)

Own source: ISC. Combined executable: GPLv3, with Corresponding Source. Four compiled interpolation files retain GPLv3/commercial terms despite ISC module declarations: `juce_GenericInterpolator.h`, `juce_Interpolators.h`, `juce_Interpolators.cpp`, `juce_WindowedSincInterpolator.cpp`. [Distribution](../../DISTRIBUTION.md), [notices](../../THIRD_PARTY.md)

## Editor search

| Evidence | Finding / bound |
| --- | --- |
| Reachable official history, removed paths included | 571 SDK commits + 93 module commits; no complete BLOCKS Code IDE. Private/deleted repositories and unreachable history excluded. |
| [Guide imported 2021-05-10](https://github.com/WeAreROLI/BLOCKS-SDK/blob/aecfb72e2b5e82ce0949fad671dc7764ebbc534c/docs/getting_started_with_blocks_code.html) | Links 0.2.3; unchanged in reachable history. Does not establish the latest application release. |

SDK reuse excludes editor UI/project handling from blocksctl's initial scope. [Published investigation](https://github.com/Xiphe/blocksctl/blob/18b642501399e5206d412731b27655f3c254035a/docs/research/source-basis.md)

## SDK contracts

| Operation | Contract / source |
| --- | --- |
| Compile | Offline; source + native prototypes + heap. Base heap 512 bytes for compile/upload; source `#heapsize: N` overrides it. [Compiler](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/littlefoot/roli_LittleFootCompiler.h) |
| Discover | Active protocol exchange/API mode; requires event dispatch. Empty topology alone cannot distinguish absence, contention or stalled dispatch. [Detector](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_MIDIDeviceDetector.cpp), [API mode](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_ConnectedDeviceGroup.cpp), [Mac host](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/examples/BlockFinder/MacOS/main.mm) |
| Upload | Public compiler, native table, topology and remote-heap synchronisation; no BLOCKS Code dependency. Temporary session stays attached until Ctrl-C. [Implementation](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_BlockImplementation.cpp) |
| Save | `save-default SOURCE` uploads and requests persistence in one process. Loaded callback supplies no flash acknowledgement; verify behaviour/power cycle. [Implementation](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/topology/internal/roli_BlockImplementation.cpp) |
| Readback | `getProgram()` returns the host object; no arbitrary device-program export. [Interface](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/blocks/roli_Block.h) |

[Offline verification](../verification/2026-11-06.md): native macOS/Linux builds, eight tests, matched-heap reference bytecode parity. [USB](../verification/2026-11-06-usb-hardware.md) and [lifecycle](../verification/2026-11-06-device-lifecycle.md): one Lightpad, firmware 1.1.0. Other firmware requires program/native-function compatibility checks; newer modules require separate validation.
