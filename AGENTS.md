# Agent entry

| Task | Read |
| --- | --- |
| Install a released binary | [macOS install](docs/install-macos.md) |
| Compile or operate a device | [Command contracts](docs/commands.md) |
| Change source or prepare a release | [Build](docs/build.md), [distribution](DISTRIBUTION.md), [dependency notices](THIRD_PARTY.md) |
| Investigate SDK behaviour | [Source basis](docs/research/source-basis.md), [domain terms](CONTEXT.md) |
| Plan hardware validation | [USB QA](docs/verification/2026-11-06-usb-hardware.md), [device lifecycle QA](docs/verification/2026-11-06-device-lifecycle.md) |

## Project constraints

- Docs assume expert humans and frontier agents. Prefer facts, code and evidence. Link external sources for general tooling; explain only blocksctl/BLOCKS-specific behaviour.
- Hardware tests need exclusive device ownership, a known restorable source program, and baseline restoration after persistent-save tests.
- Keep serials, UIDs and raw SysEx in local evidence. Publish outcomes, hashes and sanitized measurements.
- Release packaging pins the tested binary hash and compares compiled inputs with its build revision. Preserve that provenance when changing release materials.
