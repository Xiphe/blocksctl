# Command contracts

```sh
blocksctl --help
blocksctl compile controller.littlefoot -o controller.bin
blocksctl compile controller.littlefoot -o controller.h --format header
blocksctl devices --timeout 5
blocksctl upload controller.littlefoot --device SERIAL --timeout 15
blocksctl save-default controller.littlefoot --device SERIAL --timeout 15
```

| Command | Contract |
| --- | --- |
| `compile` | Offline. Raw bytecode by default; `--format header` emits `littlefoot_program[]`. Atomic output replacement; compile failure preserves prior output. |
| `devices` | Active discovery; enters API mode and can interrupt the current program, including other devices on the connection. Reports serial, UID, model, firmware and transport. |
| `upload` | Source program in, temporary device session out. Remains attached until Ctrl-C or device loss. Exit can resume the saved default. |
| `save-default` | Uploads source, requests a persistent save, waits one second, exits. Replaces the startup program. No flash acknowledgement. |

`--device` takes the serial from discovery. Close competing BLOCKS SDK hosts. Bluetooth must already be connected through the system MIDI facilities.

## Source and memory

- Upload commands recompile `.littlefoot` source; `.bin` is not an upload input.
- Includes resolve beside the source file.
- Base heap: 512 bytes for compile and upload. Arrays add storage. A source directive overrides the base:

```c
#heapsize: 256
```

Plain Littlefoot only. Editor projects and Dashboard metadata-generated variables are outside this interface. The host's program object is not a backup of unknown device bytecode.

## Session lifetime

| Event | Result |
| --- | --- |
| Syntax/include error | Fails before device discovery |
| `--timeout` | Default 10 seconds; `0 < seconds <= 3600`. Bounds discovery, plus transfer for upload commands. |
| Temporary program loaded | Timeout no longer limits session duration |
| Device loss | Session aborts; recovery needs a new explicit discovery/upload |
| Save command exits 0 | Save was requested; verify persistence with a full power cycle and physical/MIDI checks |

Retain the previous source before `save-default`. Hardware QA restores that baseline and verifies it after a second full power cycle.

## Exit status

| Code | Meaning |
| --- | --- |
| `0` | Command completed; clean Ctrl-C after a loaded temporary session |
| `1` | Compilation, file I/O or runtime error |
| `2` | Invalid arguments or output aliases the source |
| `3` | Device unavailable, discovery/transfer timeout, lock contention or disconnect |
| `130` | Interrupted before completion |

Current evidence: one Lightpad, firmware 1.1.0, macOS, USB and Bluetooth. [Hardware results](verification/2026-11-06-device-lifecycle.md).
