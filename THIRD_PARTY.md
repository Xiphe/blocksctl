# Third-party source

The project licence covers blocksctl's own code. Dependencies retain their copyright, permission and warranty notices. Keep those notices with copies of the upstream source and any distributions that require them.

## BLOCKS SDK

Source: [WeAreROLI/BLOCKS-SDK](https://github.com/WeAreROLI/BLOCKS-SDK)

Revision: `4582a4b4313e10be6f2d1412914ed8485fc7c010`

The upstream README describes this standalone SDK as ISC licensed. Its bundled JUCE core, events, audio basics and audio devices modules identify ISC in their source headers. These copies contain copyright notices for Raw Material Software Limited and other upstream contributors. Preserve the notices in the files actually used. [SDK README](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/README.md), [JUCE core header](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/SDK/juce_core/juce_core.h), [JUCE audio devices header](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/SDK/juce_audio_devices/juce_audio_devices.h)

The vendored tree also contains code under other licences. Bundled Oboe uses Apache License 2.0; bundled zlib carries its zlib licence notice. Those notices remain in the dependency source, including [Oboe's licence](vendor/sdk/juce_audio_devices/native/oboe/LICENSE) and [zlib's README](vendor/sdk/juce_core/zip/zlib/README). The ISC module declarations do not replace those ancillary notices. Build dependencies should include only what the selected target needs.

### GPLv3 interpolation files

The release audit found four additional exceptions to the module-level ISC declarations: `juce_GenericInterpolator.h`, `juce_Interpolators.h`, `juce_Interpolators.cpp` and `juce_WindowedSincInterpolator.cpp` under `vendor/sdk/juce_audio_basics/utilities/`. Their headers offer GPLv3 or commercial terms. This project uses the GPLv3 option. The tested macOS executable retains code from these files, so the combined executable is distributed under GPLv3, with complete Corresponding Source beside it. The project-authored source remains ISC licensed. [GPLv3 text](LICENSES/GPL-3.0.txt), [distribution terms](DISTRIBUTION.md)

The release package retains the original interpolation notices and the Raw Material Software ISC module headers, ROLI's ISC notice, and zlib's original header and README. A [libc++ licence and notice copy](LICENSES/libcxx.txt) is also provided for attribution to C++ library implementations. The system runtime itself is not bundled. The source of that notice copy is [LLVM's libc++ licence file](https://github.com/llvm/llvm-project/blob/llvmorg-19.1.2/libcxx/LICENSE.TXT).

## ROLI BLOCKS module

Source: [WeAreROLI/roli_blocks_basics](https://github.com/WeAreROLI/roli_blocks_basics)

Revision: `9a6df113bf570aec53d33cbf97938d8602e4ac2f`, the gitlink pinned by the standalone SDK revision above.

Licence: ISC. Copyright 2020 ROLI Ltd. The licence permits use, copying, modification and distribution with or without a fee while preserving its copyright and permission notice. [Module licence](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/LICENSE.txt), [SDK submodule configuration](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/.gitmodules)

## Dependency changes

These licence findings apply to the pinned source above. Current JUCE releases have different licensing terms. Replacing the bundled sources with another JUCE revision requires reviewing that revision's notices and updating this file. [Current JUCE licence](https://github.com/juce-framework/JUCE/blob/master/LICENSE.md)
