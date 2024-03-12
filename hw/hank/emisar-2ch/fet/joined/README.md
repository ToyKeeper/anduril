# Emisar 2-channel driver connected to only 1 set of LEDs

The default Emisar D4S driver doesn't support a lighted switch, so for lights
built with one of those, Hank puts in a 2-channel driver instead... but he only
connects 1 set of LEDs.

This is an odd hardware configuration used only for special orders, I think.

The driver itself has 3 power channels:

  1. Linear regulator 1
  2. Linear regulator 2
  3. Direct-drive FET

Channels 1 and 3 are connected to the main LEDs, while channel 2 is not
physically wired up to anything.

Two channel modes are included:

  1. Linear only
  2. Linear + DD FET

Channel mode 2 is default, and the only mode enabled after a factory reset.
Channel mode 1 is included in case anyone wants to turn off the DD FET.

