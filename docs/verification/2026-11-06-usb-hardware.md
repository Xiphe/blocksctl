# USB hardware QA

2026-11-06. Temporary-upload scope passed; no product-code defect identified.

| Configuration / artifact | Value |
| --- | --- |
| Host / device | Apple Silicon, macOS 27.2; one Lightpad `Pad BLOCK (2x2)`, firmware 1.1.0 |
| Transport / application | USB `usb/dna` only; Bluetooth disconnected; scratch Ableton Live Set; BLOCKS Code/Dashboard closed |
| Checkout | `8eb08322e3fc075bc1b4b8a6be3bec3db76e5c3d`; documentation-only changes since binary build |
| Binary build | `61f792060926d6773242bb02caf2a0adf724c2a8` |
| Binary SHA-256 | `6d6e9ff9ba33205ab839fb0f9ad8a927b750d03436d2c0ebfae922dc708249e4` |
| Temporary source SHA-256 | `d7b133dfd3da4752fa0e0838d8632e0e548ad1a4ee8cda65c890c93dd3d8271d` |
| Payload | 381 bytes; source `#heapsize: 256` |
| Payload SHA-256 | `5f8ab8bc2f8c082b65862d6ed53ad5ecb34dc148ff7c9ac2a393db7be6c9c4c9` |

Probe: channel 16, CC20 requests, CC21 acknowledgements. Temporary colours purple/cyan; saved baseline yellow/green/red feedback probe. The archived legacy navigation controller was not the baseline.

```sh
blocksctl compile temporary-feedback.littlefoot -o temporary-feedback.bin
blocksctl devices --timeout 5
blocksctl upload temporary-feedback.littlefoot --device SERIAL --timeout 15
```

| Check | Outcome |
| --- | --- |
| Baseline power cycle | Operator confirmed yellow startup. |
| Compile / discovery | Both exit 0; expected payload and device model/firmware/transport. |
| Transfer | Loaded callback; host remained attached. |
| Physical temporary program | Operator confirmed colours/Live control; 4/4 taps acknowledged. |
| Ctrl-C | Exit 0; no save-default request. |
| Default return | Operator confirmed original colours/Live control; 5/5 taps acknowledged. |

| Request-to-ACK, ms | n | Min | Median | Max |
| --- | --- | --- | --- | --- |
| Temporary | 4 | 219.081 | 219.158 | 227.033 |
| Restored baseline | 5 | 221.364 | 223.940 | 226.951 |

Initial mouse-driven ACK excluded. No missing tap ACKs, capture errors or queue drops. Live-path timings do not isolate blocksctl/device latency or establish improvement. No save-default, firmware or toolchain change.

Private evidence: exact commands/logs, source backups, timestamped MIDI; executable/source hashes checked. Raw result SHA-256: `9e410b3cbb6c27d133ec626fe6a231755e9504a0f23df8c50007a7f18c922f73`. Identifiers/project material remain private. [Published record](https://github.com/Xiphe/blocksctl/blob/18b642501399e5206d412731b27655f3c254035a/docs/verification/2026-11-06-usb-hardware.md)

First-pass exclusions: Bluetooth, new-default persistence, contention, disconnect/reconnect, other devices/firmware/platforms, clean host without ROLI software. [Lifecycle QA](2026-11-06-device-lifecycle.md) adds Bluetooth/recovery/persistence; contention and clean-host operation remain untested.
