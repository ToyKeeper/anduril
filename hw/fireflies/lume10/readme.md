# Firefly / loneoceans Lume10

Regulated buck+FET driver for Firefly lights by loneoceans.

Sort of a cross between a lume1-6af, lume-x1-attiny1616, lume-x1-avr32dd20,
and hank-lume-x1-c.

- Has the "buck plus single-level DD FET" from the lume1-6af.
  So, turbo is weird.  It's only the very top ramp level,
  and it cannot ramp down to the regulated modes.

- Has the powerbank host enable function from lume-x1-attiny1616.
  Can charge other devices with a USB-C to USB-C cable,
  when powerbank host mode is enabled

- Has the MCU and general workings of a lume-x1-avr32dd20.

- Has the aw2016 aux RGB controller from hank-lume-x1-c.
  See the [hank-lume-x1-c README](../../hank/lume-x1-c/readme.md) for details
  on the aw2016 chip.

The ramp tables are custom, shaped to put the highest medium-gear mode at
level 50/150.  Gear ratios were tested on a FF E90 Blaze, FF E12 Eclipse, and
FF E07X Cannon V2.  The E90 showed a significant "bump" at gear change, but
the E07X was totally smooth... so the top gear can't be lowered to fix the
"bump" on the E90 without causing a "dip" on the E07X.

The exact calibration may vary from one individual light to another, but
a "dip" is more visible and looks worse than a "bump", so it's calibrated to
make sure it won't have a "dip", even if it means having a "bump" sometimes.

