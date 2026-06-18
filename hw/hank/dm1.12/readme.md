# Hank / Intl-Outdoor Noctigon DM1.12

This uses the Noctigon K9.3 driver, but has very different hardware,
with one throw LED and 12 flood LEDs.  The blend modes aren't very useful.

This build target just changes the defaults to fit the hardware better:

- Enable flood-only and throw-only by default;
  disable all other channel modes.
- Default to throw only.  3C to toggle flood or throw.
- Set auto blend mode to low=throw, high=flood.
  This makes a better lightning effect.
- Use red+blue for police strobe, instead of throw+flood.
- Set better default channel modes for each strobe-group mode.

Unsure if this is safe to use for all DM1.12 hardware configurations.
It might be overpowered when using Nichia 219 flood LEDs.
If in doubt, use k9.3-219 or k9.3-nofet build instead.

Mine has an Osram LED for max throw, and NTG35 4200K for flood.
I recommend removing the 12 flood optics and the ring which holds them
in place, and using the NTG35 in mule mode (bare) instead, because
NTG35 has a pretty bad green tint in these optics.  Mule mode vastly
improves the color.  Just don't look directly at the LEDs; they will cause
eye damage.

SST-20 flood LEDs have this issue too.  Otherwise, discoloration is unlikely
to be a problem with most other LEDs.

