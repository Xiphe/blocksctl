# USB hardware QA

Verified on 6 November 2026. This records the first physical-device test of blocksctl. All checks in the agreed temporary-upload scope passed. No product-code defect was identified in this pass.

## Tested configuration

| Item | Value |
| --- | --- |
| Host | Apple Silicon Mac, macOS 27.2 |
| Device | Lightpad, reported as `Pad BLOCK (2x2)` |
| Firmware | 1.1.0 |
| Connection | USB, reported as `usb/dna` |
| Source checkout | `8eb08322e3fc075bc1b4b8a6be3bec3db76e5c3d` |
| Executable build | `61f792060926d6773242bb02caf2a0adf724c2a8`; later checkout changes were documentation only |
| Executable SHA-256 | `6d6e9ff9ba33205ab839fb0f9ad8a927b750d03436d2c0ebfae922dc708249e4` |
| Temporary source SHA-256 | `d7b133dfd3da4752fa0e0838d8632e0e548ad1a4ee8cda65c890c93dd3d8271d` |
| Temporary payload | 381 bytes, with a 256-byte heap |
| Payload SHA-256 | `5f8ab8bc2f8c082b65862d6ed53ad5ecb34dc148ff7c9ac2a393db7be6c9c4c9` |

BLOCKS Code and Dashboard were closed. Bluetooth was disconnected, and the MIDI monitor listed only the USB source. The test used a scratch Ableton Live Set. Hardware serial numbers and device UIDs are omitted from this public report.

## Checks and observations

The temporary program used MIDI channel 16, CC20 for control requests and CC21 for acknowledgements. Its purple/cyan feedback was visibly different from the saved program's yellow/green/red feedback. A source directive selected the 256-byte heap.

Commands below use `SERIAL` and generic source/output names in place of the local identifiers.

```sh
blocksctl compile temporary-feedback.littlefoot -o temporary-feedback.bin
blocksctl devices --timeout 5
blocksctl upload temporary-feedback.littlefoot --device SERIAL --timeout 15
```

| Check | Result | Evidence |
| --- | --- | --- |
| Saved-default baseline | Passed | After a full power cycle, the operator observed the saved probe's yellow startup square. |
| Offline compilation | Passed | Exit 0; 381-byte payload with a 256-byte heap. |
| Bounded USB discovery | Passed | Exit 0; reported the selected device's serial, model, firmware and transport. |
| Temporary transfer | Passed | Loaded callback arrived, and the upload process remained attached. |
| Physical temporary-program operation | Passed | The operator confirmed purple/cyan feedback and Live control. Four taps produced four matching acknowledgements, with none missing. |
| Ctrl-C shutdown | Passed | Exit 0; output confirmed the temporary session ended without a save-default request. |
| Return to saved default | Passed | The operator confirmed yellow/green/red feedback and correct Live control. Five further taps produced five matching acknowledgements, with none missing. |

The initial mouse-driven acknowledgement was excluded from the temporary tap round trips. The MIDI capture reported no errors or dropped packets.

Measured request-to-acknowledgement times were 219.081 to 227.033 ms for the four temporary-program taps, with a median of 219.158 ms. The five taps after restoration measured 221.364 to 226.951 ms, with a median of 223.940 ms. These timings describe this Live feedback test; they do not isolate blocksctl or device latency and do not establish a latency improvement.

The saved default that returned was the feedback probe, not the separately archived legacy navigation controller. No startup-default save, firmware update or toolchain change was requested in this pass.

## Evidence and limits

The private QA record contains exact commands, compile/discovery/upload logs, source backups, and a timestamped MIDI capture. Its `result.json` SHA-256 is `9e410b3cbb6c27d133ec626fe6a231755e9504a0f23df8c50007a7f18c922f73`. The executable and all recorded source-backup hashes were checked against the local files when this report was prepared. Raw evidence is retained locally because it includes hardware identifiers and project-specific material.

Still unverified:

- Bluetooth discovery and upload.
- `save-default` and persistence of a newly saved program across a power cycle.
- Lock-contention diagnostics and disconnect/reconnect abort behaviour.
- Hardware operation on Linux, other devices or other firmware versions.
- Operation on a clean host with no ROLI software installed. This test closed BLOCKS Code and Dashboard; it was not an uninstall test.

Returning to an existing saved default after Ctrl-C does not verify the separate `save-default` command. That command requires its own source upload and power-cycle test.
