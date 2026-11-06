# Bluetooth, recovery and persistence QA

2026-11-06. All scoped checks passed; no product-code defect identified. Final saved program: original yellow/green/red feedback probe, power-cycle/touch/MIDI verified. Legacy navigation controller excluded from restoration.

Host/device/artifact unchanged from [USB QA](2026-11-06-usb-hardware.md): Apple Silicon macOS 27.2, Lightpad firmware 1.1.0, blocksctl 0.1.0 binary built at `61f792060926d6773242bb02caf2a0adf724c2a8`; later changes documentation only. Binary SHA-256: `6d6e9ff9ba33205ab839fb0f9ad8a927b750d03436d2c0ebfae922dc708249e4`.

| Probe | Bytecode bytes | SHA-256 |
| --- | --- | --- |
| Purple/cyan test | 381 | `5f8ab8bc2f8c082b65862d6ed53ad5ecb34dc148ff7c9ac2a393db7be6c9c4c9` |
| Saved baseline | 378 | `74a6ec6a63d3d3ad98416faff6edd8c11b808c0c9e8d31ea32de5d4edcf1e688` |

Both heaps 256 bytes. Live/BLOCKS Code/Dashboard closed; independent MIDI responder, no Live feedback path.

## Transport and recovery

```sh
# Bluetooth-only discovery/upload; USB absent.
blocksctl devices --timeout 10
blocksctl upload test-purple-cyan.littlefoot --device SERIAL --timeout 20
# USB discovery used --timeout 5; uploads used --timeout 20.
```

Bluetooth isolation: operator unplugged USB; system/CoreMIDI showed only Bluetooth; SDK reported `bluetooth`. Discovery exit 0; loaded temporary session stayed attached; operator confirmed purple/cyan interaction.

| Loaded-session loss | Outcome | Explicit recovery |
| --- | --- | --- |
| Bluetooth disconnected | Exit 3; disconnect/session-ended diagnostic. | New discovery/upload; 4/4 pings; Ctrl-C exit 0; yellow baseline returned. |
| USB unplugged, Bluetooth disconnected | No MIDI endpoints; exit 3, same diagnostic; yellow baseline observed. | USB reconnect, new discovery/upload; 4/4 pings; Ctrl-C exit 0. |

Old processes exited before reconnect; no continuation, fallback or automatic re-upload. Transfer-time loss untested. UI disconnect-request timestamps do not measure physical loss/detection latency.

## Persistent save and restore

USB; each command exited 0 before its full power cycle; no SDK host re-uploaded afterward. Later responders exchanged MIDI only.

```sh
blocksctl save-default test-purple-cyan.littlefoot --device SERIAL --timeout 20
# Full power cycle and physical test.
blocksctl save-default baseline-with-heap.littlefoot --device SERIAL --timeout 20
# Second full power cycle and physical test.
```

| Save | Operator-confirmed startup / touch | ACKs |
| --- | --- | --- |
| Distinct test | Purple startup; purple/cyan/purple | 2/2 |
| Restore baseline | Yellow startup; green/red on release | 2/2 |

Both commands reported save requested. Persistence evidence is subsequent power-cycle behaviour, not callback/exit status; SDK supplies no flash acknowledgement.

## Evidence bounds

| Direct trial | Mode | Matched/requested |
| --- | --- | --- |
| Bluetooth temporary physical taps | Echo | 4/4 |
| Bluetooth recovery | Ping | 4/4 |
| USB before disconnect | Ping | 4/4 |
| USB recovery | Ping | 4/4 |
| Persisted test physical taps | Echo | 2/2 |
| Restored baseline physical taps | Echo | 2/2 |

Total 20/20; zero misses, extra ACKs or contamination. Passive captures: zero callback errors/queue drops; continuity across endpoint loss/power cycles unproven. Each direct trial opened a fresh subscription. Responder and Live-path timings are incomparable for latency-improvement claims.

Private command/log/source/bytecode/capture hashes checked; first USB result unchanged. Lifecycle raw result SHA-256: `6c49f38865378461e303df89580e1f3eaad32ec11bf063333e04b1221a49b29d`. Identifiers/raw SysEx remain private. [Published record](https://github.com/Xiphe/blocksctl/blob/18b642501399e5206d412731b27655f3c254035a/docs/verification/2026-11-06-device-lifecycle.md)

Untested: transfer-time disconnects, contention, long-duration reliability, other devices/firmware/OSes, clean host without ROLI software, firmware upgrades, unknown-program export.
