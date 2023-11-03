# Anduril Flashlight Firmware + FSM Flashlight UI Toolkit

Anduril is a user interface for flashlights.  It is written with FSM, a UI
toolkit for flashlights.

What is FSM?  The "SM" means "Spaghetti Monster", and the "F" can be any F word
you like, such as "Flashlight" or whatever suits your mood.  FSM also means
"Finite State Machine", which is the type of abstraction used for defining user
interfaces.  It is like a flowchart turned into executable code.

## Using Anduril

Anduril has many features, but the only things a person *really* needs to know
to use it are:

  - Click for on/off
  - Hold to change brightness

For more in-depth use, read its [user manual](docs/anduril-manual.txt)
for full details.

If you want to know what changed recently, check the [ChangeLog](ChangeLog.md).

## Flashing Firmware

Get the latest updates by flashing new firmware!

A few things are needed to flash firmware:

  - A firmware ".hex" file.  Here's
    [how to find the right .hex file](docs/which-hex-file.md).

  - A flashing adapter.  The type of adapter depends on which MCU chip your
    light uses.  It may be an UPDI 3-pin adapter (attiny1616 and newer) or an
    AVR ISP 6-pin adapter (attiny85, attiny1634).

    **UPDI** typically uses a "pogo pin" adapter, with spring-loaded pins to
    hold against copper pads on the driver circuit.

    **AVR ISP** typically has two parts -- a USB device (like a USBASP), and a
    physical adapter (like a pogo pin adapter or SOIC8 clip).  SOIC8 is the
    least convenient, and is mostly only used on old models with attiny85
    chips.

  - A firmware flashing program.  A few compatible programs include
    `avrdude`, `pymcuprog`, and `ZFlasher`.

  - A computer or phone.  Almost any modern computer or phone should be able to
    do it, as long as you can plug the adapter in and run a flashing program.

One particularly useful guide is at https://anduril.click/ .

## Compiling

The firmware can be deeply customized by modifying it and compiling your own
versions, and this is encouraged.

To compile the firmware, it is strongly recommended that you use a Linux
computer, ideally running Debian or Ubuntu (but almost any distro should work).
Virtual machines work well, such as running WSL inside Windows.  There is also
a Docker container available (TODO: add link(s) here), if that is more
convenient.

### Prerequisites:

  - AVR toolchain packages:  
    `sudo apt install gcc-avr avr-libc binutils-avr avrdude`

  - One or more Atmel DFPs (Device Family Pack) may be needed, to add support
    for recent AVR MCUs.

    - Download the Atmel ATtiny Series Device Support pack:
      http://packs.download.atmel.com/

    - Unzip the pack somewhere on your build computer

    - Set ATTINY_DFP=/path/to/where/you/unzipped/the/pack
      (either in your shell, or in this repo's bin/build.sh script)

      `export ATTINY_DFP=$HOME/src/torches/atmel/attiny-dfp`

### Building

Use the `make` script included in this repo.  Run `./make --help` for details
about how to use it.  In most cases though, you should be able to just run
`./make` by itself to compile all available build targets.  Or give it a search
term to limit builds to only a few, like `./make d4v2` to build all Emisar D4v2
firmwares.

The compiled firmware goes into the `hex/` directory, ready to be used by any
firmware flashing program.

