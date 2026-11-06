#!/usr/bin/env python3
"""Package the checked v0.1.0 binary and its complete source. No network or MIDI."""
# SPDX-License-Identifier: ISC
import argparse
import gzip
import hashlib
import io
import json
from pathlib import Path
import posixpath
import re
import struct
import subprocess
import tarfile


ROOT = Path(__file__).resolve().parents[1]


def git(*args):
    return subprocess.check_output(["git", "-C", str(ROOT), *args])


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def check_binary(data, manifest):
    if sha256(data) != manifest["binary_sha256"]:
        raise ValueError("Binary hash does not match the hardware-tested release artifact")
    header = struct.unpack_from("<8I", data)
    if header[0] != 0xFEEDFACF or header[1] != 0x0100000C:
        raise ValueError("Expected a thin arm64 Mach-O executable")
    position, minimum = 32, None
    for _ in range(header[4]):
        command, size = struct.unpack_from("<II", data, position)
        if command == 0x32:
            platform, version = struct.unpack_from("<II", data, position + 8)
            if platform != 1:
                raise ValueError("Expected the macOS platform")
            minimum = f"{version >> 16}.{(version >> 8) & 255}.{version & 255}"
        position += size
    if minimum != manifest["minimum_macos"]:
        raise ValueError("Unexpected minimum macOS version")


def document(path, revision):
    """Keep documentation links usable outside the source checkout."""
    text = (ROOT / path).read_text()

    def link(match):
        target = match.group(1)
        if "://" in target or target.startswith("#"):
            return match.group(0)
        target = posixpath.normpath(str(Path(path).parent / target))
        return "](https://github.com/Xiphe/blocksctl/blob/" + revision + "/" + target + ")"

    return re.sub(r"\]\(([^\n]+?)\)", link, text).encode()


def leading_notice(path):
    text = (ROOT / path).read_text()
    start, end = text.index("/*"), text.index("*/") + 2
    return ("Source: " + path + "\n\n" + text[start:end] + "\n").encode()


def write_gzip(path, data):
    with path.open("xb") as output:
        with gzip.GzipFile(filename="", mode="wb", fileobj=output, mtime=0) as compressed:
            compressed.write(data)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    manifest = json.loads((ROOT / "release/0.1.0.json").read_text())
    if git("status", "--porcelain").strip():
        raise ValueError("Commit all source and release inputs before packaging")
    revision = git("rev-parse", "HEAD").decode().strip()
    # Preserve provenance of the exact executable already tested on hardware.
    git("diff", "--exit-code", manifest["binary_build_revision"], revision,
        "--", "CMakeLists.txt", "src", "vendor")
    version = re.search(r"project\(blocksctl VERSION ([0-9.]+)", (ROOT / "CMakeLists.txt").read_text()).group(1)
    if version != manifest["version"]:
        raise ValueError("CMake and release versions differ")
    data = args.binary.read_bytes()
    check_binary(data, manifest)
    info = dict(manifest, release_revision=revision, build_type="Release",
                compiled_inputs_unchanged=True,
                source_archive=f"blocksctl-{version}-source.tar.gz")
    files = {
        "bin/blocksctl": data,
        "INSTALL.md": document("docs/install-macos.md", revision),
        "LICENSE": (ROOT / "LICENSES/GPL-3.0.txt").read_bytes(),
        "LICENSE-ISC.txt": (ROOT / "LICENSE").read_bytes(),
        "THIRD_PARTY.md": document("THIRD_PARTY.md", revision),
        "DISTRIBUTION.md": document("DISTRIBUTION.md", revision),
        "BUILD-INFO.json": (json.dumps(info, indent=2, sort_keys=True) + "\n").encode(),
        "notices/ROLI-ISC.txt": (ROOT / "vendor/sdk/roli_blocks_basics/LICENSE.txt").read_bytes(),
        "notices/zlib-header.txt": leading_notice("vendor/sdk/juce_core/zip/zlib/zlib.h"),
        "notices/zlib-README.txt": (ROOT / "vendor/sdk/juce_core/zip/zlib/README").read_bytes(),
        "notices/libcxx.txt": (ROOT / "LICENSES/libcxx.txt").read_bytes(),
    }
    for module in ("juce_core", "juce_events", "juce_audio_basics", "juce_audio_devices"):
        files[f"notices/{module}-ISC.txt"] = leading_notice(f"vendor/sdk/{module}/{module}.h")
    for name in ("juce_GenericInterpolator.h", "juce_Interpolators.h",
                 "juce_Interpolators.cpp", "juce_WindowedSincInterpolator.cpp"):
        files[f"notices/{name}-GPLv3.txt"] = leading_notice(f"vendor/sdk/juce_audio_basics/utilities/{name}")

    output = args.output_dir.resolve()
    if output.exists() and any(output.iterdir()):
        raise ValueError("Output directory must be new or empty; existing releases are not overwritten")
    output.mkdir(parents=True, exist_ok=True)
    name = f"blocksctl-{version}-macos-arm64"
    timestamp = int(git("show", "-s", "--format=%ct", revision))
    tar_bytes = io.BytesIO()
    with tarfile.open(fileobj=tar_bytes, mode="w", format=tarfile.PAX_FORMAT) as archive:
        for relative, content in sorted(files.items()):
            member = tarfile.TarInfo(f"{name}/{relative}")
            member.size, member.mtime = len(content), timestamp
            member.mode = 0o755 if relative == "bin/blocksctl" else 0o644
            archive.addfile(member, io.BytesIO(content))
    binary_archive = output / f"{name}.tar.gz"
    write_gzip(binary_archive, tar_bytes.getvalue())
    source_archive = output / info["source_archive"]
    write_gzip(source_archive, git("archive", "--format=tar", f"--prefix=blocksctl-{version}-source/", revision))
    sums = "".join(f"{sha256(path.read_bytes())}  {path.name}\n" for path in (binary_archive, source_archive))
    (output / "SHA256SUMS").write_text(sums)
    print(sums, end="")


if __name__ == "__main__":
    main()
