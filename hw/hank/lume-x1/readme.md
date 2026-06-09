# Hank / loneoceans Lume X1 40W

Regulated boost driver for Hank lights (Emisar / Noctigon) by loneoceans.

Second Hank light to use the new avr32dd20 MCU.  Performs better than older
MCUs and provides more space for extra features and future firmware updates.

Originally from https://github.com/loneoceans/anduril

Features UDR (Ultra Dynamic Range) for smooth dimming all the way down to
*literally zero*, meaning an effectively infinite dimming ratio:

- 3 power paths or "gears" for the regulator chip's sense resistors:
  - Firefly "gear": 0 to ~0.1 lm
  - Low "gear": ~0.1 to ~40 lm
  - High "gear": ~40 lm to full power (2k or 3k lm?)
- Internal resolution of 2500 steps per "gear" allows very precise control over
  brightness.  Anduril's 150 step ramp does not fully take advantage of this.
- Constant current, no pulsing or flicker or ripple
- No preflash
- No need for jump start; regulator has a quick response at all levels

The firefly part of the ramp is compressed to just a few levels, to avoid
making the rest of the ramp artificially low.  It aims to be reasonably
consistent compared to the ramp shape of Hank's other lights, but a few levels
at the bottom are set aside for moon and firefly modes.

The regulator can go much, much lower than the brightness of the button LEDs,
but there isn't much point because the glowing button overpowers the main LEDs.
The button and aux LEDs (on low) also use significantly less power despite
being brighter, because they do not require the MCU to be awake.  So that part
of the ramp is mostly omitted.  However, it can be added with a firmware
modification if desired.

