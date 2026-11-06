# Source build

| Requirement | macOS | Linux |
| --- | --- | --- |
| Toolchain | C++17 and Objective-C++17; [Apple tools](https://developer.apple.com/xcode/resources/), accepted licence | C++17; [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/) |
| Build/checks | [CMake](https://cmake.org/cmake/help/latest/manual/cmake.1.html) 3.16+, [Python](https://www.python.org/downloads/) 3 | Same |
| Platform dependencies | System frameworks | ALSA headers and pkg-config; Ubuntu `libasound2-dev pkg-config` |
| CI | macOS 15, arm64 | Ubuntu 24.04, x86-64 |

From a checkout or the release's [Corresponding Source archive](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/blocksctl-0.1.0-source.tar.gz):

```sh
python3 scripts/verify_vendor.py
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/blocksctl --version
```

Dependencies are vendored. `verify_vendor.py` checks retained upstream source against its manifest. CTest runs eight offline checks; device operations require separate [hardware verification](verification/2026-11-06-device-lifecycle.md).

Executable: `build/blocksctl`. Intel Mac source builds are unverified. [CI](https://github.com/Xiphe/blocksctl/actions/workflows/build.yml) · [CMake configuration](../CMakeLists.txt) · [Distribution terms](../DISTRIBUTION.md)
