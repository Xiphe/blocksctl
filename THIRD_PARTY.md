# Dependencies

## Pinned source

| Dependency | Revision | Notices |
| --- | --- | --- |
| [BLOCKS-SDK](https://github.com/WeAreROLI/BLOCKS-SDK) | `4582a4b4313e10be6f2d1412914ed8485fc7c010` | Bundled JUCE 6.0.8 modules; file-level exceptions below |
| [roli_blocks_basics](https://github.com/WeAreROLI/roli_blocks_basics) | `9a6df113bf570aec53d33cbf97938d8602e4ac2f` | [ROLI ISC](vendor/sdk/roli_blocks_basics/LICENSE.txt); SDK-pinned gitlink |

## File-level terms

| Code | Terms / retained source |
| --- | --- |
| JUCE core, events, audio basics, audio devices | Raw Material Software ISC module headers, with exceptions below |
| Four JUCE interpolator files | GPLv3/commercial headers; this binary selects [GPLv3](LICENSES/GPL-3.0.txt) |
| Bundled zlib 1.2.3 | [Header](vendor/sdk/juce_core/zip/zlib/zlib.h), [README](vendor/sdk/juce_core/zip/zlib/README) |
| Oboe, Android source only | [Apache 2.0](vendor/sdk/juce_audio_devices/native/oboe/LICENSE) |
| C++ library attribution | [libc++ notices](LICENSES/libcxx.txt), copied from [LLVM 19.1.2](https://github.com/llvm/llvm-project/blob/llvmorg-19.1.2/libcxx/LICENSE.TXT); system runtime not bundled |

The four interpolator files are under `vendor/sdk/juce_audio_basics/utilities/`:

```text
juce_GenericInterpolator.h
juce_Interpolators.h
juce_Interpolators.cpp
juce_WindowedSincInterpolator.cpp
```

Their code remains in the executable. Module-level ISC declarations do not make the combined binary ISC-only. [Binary distribution and source assets](DISTRIBUTION.md).

Dependency updates require a new file-level notice audit. These findings apply to the pinned copies, not arbitrary [current JUCE versions](https://github.com/juce-framework/JUCE/blob/master/LICENSE.md).
