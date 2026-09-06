# Hank / loneoceans Lume X1 C

Regulated boost driver for Hank lights (Emisar / Noctigon) by loneoceans.

Mostly the same as a Lume X1, but the "C" model uses an AW2016 chip to
control the RGB aux LEDs.  It runs independently of the main MCU, and uses
TWI / I2C to communicate.

The AW2016 has 3 dimming methods:

- The entire chip can run at 5 mA, 10 mA, 15 mA, or 30 mA output.
- Each of the R/G/B channels can run at 0/15ths to 15/15ths of the total
  power level.  This is used mostly for color balancing, so R, G, and B might
  run at powers 15, 4, and 5.  However, it can also be used for coarse
  dimming.
- Each of the R/G/B channels uses 8-bit PWM at 250 Hz.

The lowest usable level without PWM is 5 mA at balance levels 4:1:1.  This is
way too high to work as an aux standby "low" mode, since it still uses over
1.3 mA of power.  Aux low really should be 0.1 mA or less.

So the aux low modes must use PWM.

The 250 Hz PWM is obnoxiously slow.  Rather than making the LEDs look dim, it
makes them look like strobe lights.  There is nothing I can do about this.

Also, even with the lowest power level and a low PWM duty cycle, it still
uses more power than it should.  It uses about 0.3 mA instead of 0.1 mA.
Standby time will thus be significantly shorter, and this type of driver
isn't very practical on lights with smaller batteries, like 14500.

PWM is also required when doing smooth dimming at higher brightness levels,
since the no-PWM methods are extremely coarse.  So expect to see strobing
during RGB effects like Smooth POVD and the blended rainbow standby mode.

In multiple ways, this is a downgrade from older aux RGB designs, which
simply drove the aux LEDs directly from MCU pins, with fast PWM.  The core
function of the aux LEDs (i.e. replacing tritium vials, or glowing gently in
the dark and showing battery status without significantly affecting standby
time) is significantly worse with this design.

However, it does have some benefits:

- Balancing the colors is a bit easier, since it happens in software instead
  of requiring physical resistors.
- Colors stay balanced when dimmed, so red won't be so hard to see on low
  mode.
- It allows the user to configure the brightness of the high and low aux
  levels (with coarse resolution).
- Active RGB modes can be somewhat brighter if desired.  The maximum power is
  still pretty low though.  They are still not viable as primary LEDs.
- Very simple "breathing" animations are possible while the MCU is asleep.
  The "breathing" for each channel can also be offset to make a rainbow
  cycle.  It will, however, be much brighter than an aux "low" mode, even at
  the lowest brightness, because the internal animation system requires the
  full PWM range of 0 to 255.  It cannot be dimmed to run at, for example,
  0 to 32.

