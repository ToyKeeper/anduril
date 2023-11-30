# Lumintop FW3X Lume1

A BLF FW3A with a new driver from LoneOceans.  The new driver adds efficient
constant-current power regulation, RGB aux LEDs, and an upgraded temperature
sensor.

## Notes of interest

**Flashing firmware**:  The MOSI and MISO pin are swapped, compared to a Hanklight.
LoneOceans sent a fixed driver design to Lumintop, but the new design didn't
get produced.  So to flash firmware, swap the wires for those two pins first.

**RGB mixup**: Lumintop seems to have swapped the wires for aux R and aux B.
This was fixed in firmware in 2023-12, but some lights were fixed in hardware
before that, so the firmware fix might cause the colors to be swapped again.

**Turbo**: The driver uses regulation for levels 1 to 149, and level 150 is a
direct-drive FET.  This is by design, and the FET cannot be ramped smoothly up
and down.  Turbo is a single level at the top of the ramp, with a big sudden
drop to the next level.

**Moon**: This light has a pretty bright preflash at moon level, and the
output is unstable so there is very visible ripple.  The user can either raise
the ramp floor to a level high enough to avoid these issues, or learn to live
with the ripple and preflash.

