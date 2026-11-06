#!/usr/bin/env python3
"""Check the retained upstream sources against the recorded file hashes."""
# SPDX-License-Identifier: ISC
import hashlib
import json
from pathlib import Path
import sys

vendor = Path(__file__).resolve().parents[1] / "vendor"
manifest = json.loads((vendor / "manifest.json").read_text())
expected = manifest["sha256"]
actual = {
    str(path.relative_to(vendor)): hashlib.sha256(path.read_bytes()).hexdigest()
    for path in (vendor / "sdk").rglob("*") if path.is_file()
}
changed = sorted(name for name in expected.keys() | actual.keys() if expected.get(name) != actual.get(name))
if changed:
    print("Vendored source differs from its manifest:\n" + "\n".join(changed), file=sys.stderr)
    sys.exit(1)
print(f"Verified {len(expected)} upstream source files.")
