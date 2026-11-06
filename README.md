# blocksctl

**Littlefoot on your Lightpad.** Compile, upload and save from your terminal. No ROLI desktop apps or account.

[Download v0.1.0](https://github.com/Xiphe/blocksctl/releases/tag/v0.1.0) · [Install](docs/install-macos.md) · [Commands](docs/commands.md)

Apple Silicon · macOS 15+ · USB & Bluetooth

```sh
blocksctl compile controller.littlefoot -o controller.bin
blocksctl devices --timeout 5
blocksctl upload controller.littlefoot --device SERIAL
```

Discovery enters device API mode. `upload` holds the connection until Ctrl-C. `save-default` writes the startup program.

## Bring your agent

Point it at [AGENTS.md](AGENTS.md). For source work, start with [Build](docs/build.md).

## Tested

Lightpad, firmware 1.1.0, macOS. USB, Bluetooth, reconnect recovery, persistent save and baseline restoration passed [hardware QA](docs/verification/2026-11-06-device-lifecycle.md). [Compiler checks](docs/verification/2026-11-06.md) pass on macOS and Linux.

Our code is [ISC](LICENSE). The release binary is [GPLv3](DISTRIBUTION.md).
