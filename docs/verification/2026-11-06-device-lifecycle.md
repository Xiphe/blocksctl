# Bluetooth, recovery and persistence QA

Verified on 6 November 2026. All requested tests passed, and the operator confirmed the original saved baseline was restored after a full power cycle. No blocksctl product-code defect was identified in this pass.

This follows the [first USB hardware pass](2026-11-06-usb-hardware.md). That pass's raw result file remained unchanged.

## Tested configuration

The host, device and executable were unchanged from the first pass: an Apple Silicon Mac with macOS 27.2, one Lightpad with firmware 1.1.0, and blocksctl 0.1.0. The executable SHA-256 remained `6d6e9ff9ba33205ab839fb0f9ad8a927b750d03436d2c0ebfae922dc708249e4`. It was built from source revision `61f792060926d6773242bb02caf2a0adf724c2a8`; later project changes were documentation only.

The saved baseline was a yellow/green/red feedback probe. The test source used purple/cyan colours so that physical observations could distinguish the two programs. Both source programs selected a 256-byte heap. The previously archived navigation controller was not used as the restoration baseline.

| Source program | Bytecode size | Bytecode SHA-256 |
| --- | --- | --- |
| Purple/cyan test probe | 381 bytes | `5f8ab8bc2f8c082b65862d6ed53ad5ecb34dc148ff7c9ac2a393db7be6c9c4c9` |
| Saved baseline probe | 378 bytes | `74a6ec6a63d3d3ad98416faff6edd8c11b808c0c9e8d31ea32de5d4edcf1e688` |

Live, BLOCKS Code and Dashboard were closed for these tests. A separate MIDI responder handled request/acknowledgement checks. Commands below use generic source names and `SERIAL` in place of local paths and hardware identifiers.

## Bluetooth-only operation

USB was disconnected, the system showed a Bluetooth connection, CoreMIDI exposed only the Bluetooth connection, and SDK discovery reported `bluetooth`.

```sh
blocksctl devices --timeout 10
blocksctl upload test-purple-cyan.littlefoot --device SERIAL --timeout 20
```

Discovery exited 0. Temporary upload reached the loaded state and stayed attached. The operator confirmed the expected purple/cyan behaviour. Four physical taps produced four matching acknowledgements, with none missing or extra.

## Disconnect and explicit recovery

| Transport | Loss during an already loaded session | Recovery |
| --- | --- | --- |
| Bluetooth | The old upload process exited 3 and reported that the device disconnected and the temporary session ended. | A new explicit discovery and upload succeeded. All four automated MIDI pings were acknowledged. Ctrl-C exited 0, and the operator observed the saved yellow baseline return. |
| USB | With Bluetooth still disconnected, unplugging USB left no MIDI connections. The upload process exited 3 with the same disconnect message. | After reconnecting USB, a new discovery and upload succeeded. All four automated MIDI pings were acknowledged. Ctrl-C exited 0. The operator also observed the saved yellow baseline after the loss of the first session. |

The old sessions had exited before reconnection. There was no silent continuation or automatic re-upload by those processes. The USB program had also acknowledged all four automated pings before the disconnect.

These checks cover transport loss after upload completion. They do not cover a disconnect during transfer. The recorded UI disconnect-request time is not a calibrated physical link-loss timestamp, so no precise disconnect-detection latency is claimed.

## Persistent save and baseline restoration

Persistence was tested over USB, separately from Bluetooth recovery.

```sh
blocksctl save-default test-purple-cyan.littlefoot --device SERIAL --timeout 20
# After the command exits, fully turn the device off and on, then verify it.
blocksctl save-default baseline-with-heap.littlefoot --device SERIAL --timeout 20
# After restoration exits, fully turn the device off and on and verify again.
```

| Stage | Command result | Observation after full power cycle |
| --- | --- | --- |
| Save distinct test probe | Exit 0; output reported that save-default was requested. | Purple startup returned without an SDK host uploading it. The operator confirmed purple/cyan/purple feedback on two taps, with two matching MIDI acknowledgements. |
| Restore saved baseline | Exit 0; output reported that save-default was requested. | Yellow startup returned without an SDK host uploading it. The operator confirmed green then red on touch release, with two matching MIDI acknowledgements. |

Both save processes had exited before the corresponding power cycle. The later MIDI responders handled messages only and did not upload source programs. The device was left with the original saved baseline, verified by startup colour and touch/MIDI behaviour.

The SDK still supplies no flash-save acknowledgement. The persistence result rests on the full power cycles and subsequent physical/MIDI observations, not on the loaded callback or command exit alone.

## Evidence and limits

Private evidence includes command records, stdout/stderr logs, source and bytecode files, isolated responder trials and passive MIDI captures. The raw `remaining-tests/result.json` SHA-256 is `6c49f38865378461e303df89580e1f3eaad32ec11bf063333e04b1221a49b29d`. Source/bytecode/capture hashes and the unchanged first-pass result were checked locally when this report was prepared. Hardware identifiers and raw SysEx remain in the local evidence.

All six direct MIDI trials completed without misses, extra acknowledgements or recorded contamination: four Bluetooth physical taps, four Bluetooth recovery pings, four USB pre-disconnect pings, four USB recovery pings, two persisted-test physical taps and two restored-baseline physical taps. The stopped passive captures reported no callback errors or queue drops. They are not evidence of uninterrupted capture across endpoint loss or power cycles; each direct trial opened its own subscription.

The independent responder differs from the first pass's Live feedback path. Timing measurements from the two setups do not establish that blocksctl improved latency.

Still outside this hardware verification:

- Disconnect during transfer, lock contention and long-duration reliability.
- Other physical devices, firmware versions and operating systems.
- A clean host with no ROLI software installed. Closing the editor and Dashboard is not an uninstall test.
- Firmware upgrades and exporting an unknown saved program from a device.
