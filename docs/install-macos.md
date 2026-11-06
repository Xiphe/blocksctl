# Install blocksctl on macOS

The [v0.1.0 release](https://github.com/Xiphe/blocksctl/releases/tag/v0.1.0) provides an Apple Silicon `arm64` binary for macOS 15.0 or later. Intel Macs need a source build. This release has no prebuilt Intel Mac binary.

Running the downloaded binary needs neither Xcode nor an accepted Xcode licence. It also needs no BLOCKS Code, Dashboard, Connect, vendor account or ROLI server connection. Device commands use the device's existing firmware. Hardware checks covered one Lightpad with firmware 1.1.0, over USB and Bluetooth; other firmware and models need separate verification.

## Download and verify

Download these two files from the same release into an empty directory:

- [blocksctl-0.1.0-macos-arm64.tar.gz](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/blocksctl-0.1.0-macos-arm64.tar.gz)
- [SHA256SUMS](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/SHA256SUMS)

In Terminal, change to that directory and check the archive before extracting or running it:

```sh
shasum -a 256 blocksctl-0.1.0-macos-arm64.tar.gz
cat SHA256SUMS
```

Compare the first command's hash with the entry for `blocksctl-0.1.0-macos-arm64.tar.gz` in `SHA256SUMS`. They must match exactly. The manifest also lists a source archive; downloading that archive is optional for installation. A mismatch means the downloaded archive does not match the release manifest. Download it again from the release page before proceeding. The checksum detects a mismatch; it is not an Apple identity signature.

Extract the verified archive:

```sh
tar -xzf blocksctl-0.1.0-macos-arm64.tar.gz
```

The extracted `blocksctl-0.1.0-macos-arm64` directory contains `bin/blocksctl`, installation instructions, distribution terms, project and dependency licence notices, and `BUILD-INFO.json`.

The executable's SHA-256 is:

```text
6d6e9ff9ba33205ab839fb0f9ad8a927b750d03436d2c0ebfae922dc708249e4
```

You can compare it with `shasum -a 256 blocksctl-0.1.0-macos-arm64/bin/blocksctl`. This is the exact executable used for the release's recorded hardware tests.

## Binary licence and source

The combined executable contains GPLv3 JUCE code and is distributed under GPLv3. blocksctl's own source is ISC licensed. The release includes the applicable licence texts and notices.

The [complete Corresponding Source archive](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/blocksctl-0.1.0-source.tar.gz) is available from the same release, with its checksum in `SHA256SUMS`. It includes the source and build materials for this binary. You do not need to download or compile it to run the macOS release. If you download both archives, `shasum -a 256 -c SHA256SUMS` checks both together. [Distribution terms](https://github.com/Xiphe/blocksctl/blob/v0.1.0/DISTRIBUTION.md)

## Install for your user

From the directory containing the extracted release:

```sh
mkdir -p "$HOME/.local/bin"
install -m 755 blocksctl-0.1.0-macos-arm64/bin/blocksctl "$HOME/.local/bin/blocksctl"
"$HOME/.local/bin/blocksctl" --version
```

The expected output is `blocksctl 0.1.0`. The installation writes only to your user's directory and needs no `sudo`. Keep the extracted notices and build information with your release download.

To use `blocksctl` by name in the current Terminal session:

```sh
export PATH="$HOME/.local/bin:$PATH"
blocksctl --version
```

For future zsh sessions, add that `export` line to `~/.zshrc` once, if your configuration does not already add `$HOME/.local/bin`. Open a new Terminal session afterward. You can always use the explicit path `"$HOME/.local/bin/blocksctl"` without changing `PATH`.

## If macOS blocks the executable

This release has an ad-hoc code signature. It has no Developer ID signature and has not been notarized by Apple. macOS may block it when downloaded through a browser.

After verifying the checksum and deciding to trust this release, try the version command above. If macOS blocks it, open System Settings, choose Privacy & Security, and look for the blocked `blocksctl` item. Use **Open Anyway** for that item and confirm the prompt, then retry the version command. Apple documents this per-item process in [Safely open apps on your Mac](https://support.apple.com/en-us/102445).

## First commands

Compile a local program without contacting hardware:

```sh
blocksctl compile program.littlefoot -o program.bin
```

To work with a connected device, close other BLOCKS SDK hosts, then actively discover it:

```sh
blocksctl devices --timeout 5
```

Discovery sends protocol messages and can enter API mode. Copy the device's reported serial into the next command:

```sh
blocksctl upload program.littlefoot --device SERIAL --timeout 15
```

This uploads a temporary program and keeps the host session running. Press Ctrl-C to end it. The timeout bounds discovery and transfer, not the duration of a successfully loaded session.

To replace the device's saved startup program:

```sh
blocksctl save-default program.littlefoot --device SERIAL --timeout 15
```

This overwrites the previous default. Retain its source if you need to restore it. After the command exits, fully power the device off and on and verify the new startup behaviour. The command reports a save request; the SDK does not acknowledge the flash write.

## Remove blocksctl

Delete `~/.local/bin/blocksctl` to remove this installation. Removing the host executable does not change a program saved on a device.
