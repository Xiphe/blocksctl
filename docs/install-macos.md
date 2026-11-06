# macOS install

[v0.1.0](https://github.com/Xiphe/blocksctl/releases/tag/v0.1.0): Apple Silicon, macOS 15.0+. Runtime uses system frameworks; Xcode and ROLI applications/accounts are unnecessary. Intel: [source build](build.md).

```sh
(
set -eu
blocksctl_install_dir=$(mktemp -d)
cd "$blocksctl_install_dir"
blocksctl_release_url=https://github.com/Xiphe/blocksctl/releases/download/v0.1.0
curl -fsSLO "$blocksctl_release_url/blocksctl-0.1.0-macos-arm64.tar.gz"
curl -fsSLO "$blocksctl_release_url/SHA256SUMS"
awk '$2 == "blocksctl-0.1.0-macos-arm64.tar.gz"' SHA256SUMS > binary.sha256
test -s binary.sha256
shasum -a 256 -c binary.sha256
tar -xzf blocksctl-0.1.0-macos-arm64.tar.gz
mkdir -p "$HOME/.local/bin" "$HOME/.local/share/blocksctl/0.1.0"
cp -R blocksctl-0.1.0-macos-arm64/. "$HOME/.local/share/blocksctl/0.1.0/"
install -m 755 "$HOME/.local/share/blocksctl/0.1.0/bin/blocksctl" "$HOME/.local/bin/blocksctl"
"$HOME/.local/bin/blocksctl" --version
)
```

Expected: archive `OK`, then `blocksctl 0.1.0`. The full package, notices and `BUILD-INFO.json` remain in `~/.local/share/blocksctl/0.1.0`.

Current shell:

```sh
export PATH="$HOME/.local/bin:$PATH"
blocksctl --version
```

Ad-hoc signed; no Developer ID or notarization. If blocked, use Apple's per-item [Open Anyway](https://support.apple.com/en-us/102445) after verifying the release.

Own source: ISC. Combined binary: GPLv3. [Distribution terms](../DISTRIBUTION.md), [Corresponding Source](https://github.com/Xiphe/blocksctl/releases/download/v0.1.0/blocksctl-0.1.0-source.tar.gz).

[Commands](commands.md) · [Release evidence and limits](releases/0.1.0.md)
