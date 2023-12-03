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

For more in-depth use, read its [user manual](docs/anduril-manual.md)
for full details.

If you want to know what changed recently, check the [ChangeLog](ChangeLog.md).

For info about hardware-specific features, like what the channel modes are on
multi-channel lights, browse into the [hw/](hw/) directories to find the
hardware model and any hardware-specific documentation.


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

More info about installing flashing programs is below, in
[Flashing Programs](#flashing-programs).


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
  `sudo apt install gcc-avr avr-libc binutils-avr`

- Other misc packages:  
  `sudo apt install git wget unzip bash`

- Atmel DFPs (Device Family Packs).  A small script is included to
  download and install these for you:  
  `./make dfp`

### Building

Use the `make` script included in this repo.  Run `./make --help` for details
about how to use it.  In most cases though, you should be able to just run
`./make` by itself to compile all available build targets.  Or give it a search
term to limit builds to only a few, like `./make hank boost` to build all
firmwares for Hanklights with optional boost drivers.

The compiled firmware goes into the `hex/` directory, ready to be used by any
firmware flashing program.


## Customizing Settings Per User

The build can be [customized per user](docs/per-user-config.md) by placing
overrides into the `users/myname/` directory and letting the build script know
your username.  That way, your favorite settings can be applied automatically
without having to modify the original source files.


## Flashing Programs

### AVRdude

Usually avrdude installs in a single command:

`sudo apt install avrdude`

### PyMCUprog

If you use `pymcuprog` to flash firmware, a few extras are needed:

```sh
sudo apt install python3 python3-pip python3-venv
python3 -m venv .venv
source .venv/bin/activate
pip install pymcuprog
```

You'll need to `source .venv/bin/activate` every time you start a fresh shell,
if you want to use pymcuprog.  The activation lasts until the shell is
closed or until you run `deactivate`.


## Contributing

If you'd like to help, there are several ways...

  - ToyKeeper has a [Patreon site](https://patreon.com/ToyKeeper) for
    donations, to help figuratively and literally keep the lights on.

  - Send pull requests or patches, to improve things directly.  These can be
    code, documentation, tools, or anything you think is useful.

  - File bug reports for any problems you find.

  - Answer people's questions on public forums.  If the same questions come up
    a lot, consider adding it to the documentation here.

  - Ask manufacturers to use this firmware, and to support its development.

