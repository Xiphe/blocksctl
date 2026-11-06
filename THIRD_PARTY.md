# Third-party source

The project licence covers blocksctl's own code. Dependencies retain their copyright, permission and warranty notices. Keep those notices with copies of the upstream source and any distributions that require them.

## BLOCKS SDK

Source: [WeAreROLI/BLOCKS-SDK](https://github.com/WeAreROLI/BLOCKS-SDK)

Revision: `4582a4b4313e10be6f2d1412914ed8485fc7c010`

The upstream README describes this standalone SDK as ISC licensed. Its bundled JUCE core, events, audio basics and audio devices modules identify ISC in their source headers. These copies contain copyright notices for Raw Material Software Limited and other upstream contributors. Preserve the notices in the files actually used. [SDK README](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/README.md), [JUCE core header](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/SDK/juce_core/juce_core.h), [JUCE audio devices header](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/SDK/juce_audio_devices/juce_audio_devices.h)

The vendored tree also contains code under other licences. Bundled Oboe uses Apache License 2.0; bundled zlib carries its zlib licence notice. Those notices remain in the dependency source, including [Oboe's licence](vendor/sdk/juce_audio_devices/native/oboe/LICENSE) and [zlib's README](vendor/sdk/juce_core/zip/zlib/README). The ISC module declarations do not replace those ancillary notices. Build dependencies should include only what the selected target needs.

## ROLI BLOCKS module

Source: [WeAreROLI/roli_blocks_basics](https://github.com/WeAreROLI/roli_blocks_basics)

Revision: `9a6df113bf570aec53d33cbf97938d8602e4ac2f`, the gitlink pinned by the standalone SDK revision above.

Licence: ISC. Copyright 2020 ROLI Ltd. The licence permits use, copying, modification and distribution with or without a fee while preserving its copyright and permission notice. [Module licence](https://github.com/WeAreROLI/roli_blocks_basics/blob/9a6df113bf570aec53d33cbf97938d8602e4ac2f/LICENSE.txt), [SDK submodule configuration](https://github.com/WeAreROLI/BLOCKS-SDK/blob/4582a4b4313e10be6f2d1412914ed8485fc7c010/.gitmodules)

## Dependency changes

These licence findings apply to the pinned source above. Current JUCE releases have different licensing terms. Replacing the bundled sources with another JUCE revision requires reviewing that revision's notices and updating this file. [Current JUCE licence](https://github.com/juce-framework/JUCE/blob/master/LICENSE.md)
