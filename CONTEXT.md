# Littlefoot programs on BLOCKS

This project concerns compiling Littlefoot programs and loading them onto ROLI BLOCKS devices. The first target is the Lightpad BLOCK.

## Language

**Device**:
A physical ROLI BLOCKS unit identified by its serial number.
_Avoid_: Target, endpoint

**Source program**:
A program written in the Littlefoot language, before compilation.
_Avoid_: Project, app

**Bytecode**:
The compiled form of a Littlefoot program that a compatible device can execute.
_Avoid_: Firmware, executable

**Discovery**:
An active exchange that identifies connected BLOCKS devices and their properties.
_Avoid_: Passive scan, endpoint listing

**Upload**:
The transfer of a program to a selected device for temporary use.
_Avoid_: Install, save

**Temporary program**:
A program loaded for the current device session without changing the saved default.
_Avoid_: Installed program, default

**Saved default**:
The program stored on a device for use after it starts again.
_Avoid_: Factory program, temporary program
