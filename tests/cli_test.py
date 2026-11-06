#!/usr/bin/env python3
"""Exercise real offline compiler output and failures. Never discover hardware."""
# SPDX-License-Identifier: ISC
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest

BINARY = Path(sys.argv.pop(1)).resolve()
FIXTURE = Path(__file__).parent / "fixtures/controller.littlefoot"


class CliTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def run_cli(self, *args, code=0, cwd=None):
        result = subprocess.run(
            [str(BINARY), *map(str, args)], cwd=cwd or self.root,
            capture_output=True, text=True, timeout=10,
        )
        self.assertEqual(result.returncode, code, result.stdout + result.stderr)
        self.assertNotIn("Active BLOCKS discovery", result.stderr)
        return result

    def payload(self, path):
        data = path.read_bytes()
        self.assertGreaterEqual(len(data), 10)
        checksum, size, functions, globals_, heap = struct.unpack_from("<5H", data)
        self.assertEqual(size, len(data))
        actual = size
        for byte in data[2:]:
            actual = (actual * 3 + byte) & 0xFFFF
        self.assertEqual(checksum, actual)
        self.assertGreater(functions, 0)
        return data, heap

    def test_binary_header_and_repeat_match(self):
        binary, repeat, header = [self.root / name for name in ("out.bin", "repeat.bin", "out.h")]
        self.run_cli("compile", FIXTURE, "-o", binary)
        self.run_cli("compile", FIXTURE, "-o", repeat)
        self.run_cli("compile", FIXTURE, "-o", header, "--format", "header")
        data, heap = self.payload(binary)
        self.assertEqual(heap, 512)
        self.assertEqual(data, repeat.read_bytes())
        decoded = bytes(int(item, 16) for item in re.findall(r"0x([0-9a-f]{2})", header.read_text()))
        self.assertEqual(data, decoded)

    def test_source_heap_directive(self):
        source, output = self.root / "heap.littlefoot", self.root / "heap.bin"
        source.write_text("#heapsize: 256\n" + FIXTURE.read_text())
        self.run_cli("compile", source, "-o", output)
        self.assertEqual(self.payload(output)[1], 256)

    def test_includes_resolve_beside_source(self):
        source_dir = self.root / "source"
        source_dir.mkdir()
        (source_dir / "helper.littlefoot").write_text("int colour() { return 0xff2255aa; }\n")
        source = source_dir / "main.littlefoot"
        source.write_text('#include "helper.littlefoot"\nvoid repaint() { fillPixel(colour(), 0, 0); }\n')
        output = self.root / "include.bin"
        self.run_cli("compile", source, "-o", output)
        self.payload(output)

    def test_failures_preserve_output(self):
        output = self.root / "previous.bin"
        output.write_bytes(b"last good program")
        bad = self.root / "bad.littlefoot"
        bad.write_text("void repaint() { this is not valid; }\n")
        for source in (bad, self.root / "missing.littlefoot"):
            self.run_cli("compile", source, "-o", output, code=1)
            self.assertEqual(output.read_bytes(), b"last good program")
        self.assertEqual(list(self.root.glob("previous.bin.*")), [])

    def test_source_and_aliases_cannot_be_output(self):
        source = self.root / "source.littlefoot"
        original = FIXTURE.read_bytes()
        source.write_bytes(original)
        symlink, hardlink = self.root / "symlink", self.root / "hardlink"
        symlink.symlink_to(source)
        os.link(source, hardlink)
        for output in (source, symlink, hardlink):
            self.run_cli("compile", source, "-o", output, code=2)
            self.assertEqual(source.read_bytes(), original)

    def test_invalid_source_upload_fails_before_device_access(self):
        bad = self.root / "bad.littlefoot"
        bad.write_text("void repaint() { invalid syntax here; }\n")
        for command in ("upload", "save-default"):
            self.run_cli(command, bad, "--device", "not-a-real-serial", code=1)

    def test_invalid_arguments_fail_before_device_access(self):
        cases = [
            (), ("unknown",), ("compile", FIXTURE),
            ("compile", FIXTURE, "-o", self.root / "x", "--format", "wrong"),
            ("upload", FIXTURE), ("save-default", "--device", "serial"),
            ("devices", "--timeout", "nan"), ("devices", "--timeout", "inf"),
            ("devices", "--timeout", "0"), ("devices", "--timeout", "no"),
            ("devices", "--device", "serial"),
        ]
        for args in cases:
            with self.subTest(args=args):
                self.run_cli(*args, code=2)

    def test_help_and_version_are_offline(self):
        self.assertIn("blocksctl", self.run_cli("--version").stdout)
        self.assertIn("save-default SOURCE", self.run_cli("--help").stdout)
        self.run_cli("devices", "--help")


if __name__ == "__main__":
    unittest.main()
