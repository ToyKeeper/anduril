# FireflyLite (Fireflies) Lume-series drivers

Some FireflyLite flashlights use the Lume series of drivers by Loneoceans.
These drivers use efficient constant-current power regulation with no PWM,
feature Ultra Dynamic Range for extremely low moon modes, and may include
other features such as USB-C charging. 

The choice of firmware to use may not depend on the flashlight model 
since multiple flashlights are compatible with different drivers. The 
best way to tell is to unscrew the flashlight body and read the driver 
model, revision number, and date code, printed on the driver PCB.

## Lume1-6AF

Lume1-6AF is a 6A constant-current buck driver with FET for turbo. This
driver was first released in 2020. The driver is used with single-die 
emitters or paralleled emitters (3V configurations). Information for 
previous versions can be found here: 
[https://github.com/loneoceans/lume1-ff-6af](https://github.com/loneoceans/lume1-ff-6af).

Use the following .hex files:

* Rev B 11/20 - fireflies-lume1-2020.hex (HWID 0443)
* Rev C 10/22 - fireflies-lume1-6af.hex (HWID 0451)
* Rev D 10/23 - same as Rev C

Some flashlights may use this driver with a single small emitter, such as 
an Osram CSLNM1.TG in the T9R or T1R, or a FFL505A in the T1R or X1S. The 
FET cannot be used as it will over-drive the emitter. Use the no-fet 
firmware variant in this case:

* Rev B 11/20 - fireflies-lume1-2020-nofet.hex (HWID 0444)
* Rev C 10/22 - fireflies-lume1-6af-nofet.hex (HWID 0452)
* Rev D 10/23 - same as Rev C

Typical flashlights that use the Lume1-6AF driver include the E07X Pro, 
E12R, NOV-Mu, T9R, and T1R. 

Rev B drivers use the Attiny1634 MCU. Rev C and D use the Attiny1616 MCU. All
versions feature an intelligent 2A battery charger, with the power increased 
to 10W for Rev D.

## Lume-X1-40W

Lume-X1-40W is a 40W constant-current boost driver. This driver was released
in early 2023. All flashlights with the Lume-X1 driver use the same firmware
regardless of output configuration (usually 6V for XHP70.3 emitters, or 12V
for XHP35HI emitters).

* Rev A2 01/23 - fireflies-lume-x1-40w.hex (HWID 0461)

Typical flashlights that use the Lume-X1-40W driver include the X1L and X1S,
and may also be used in the T9R or T1R. This driver should not be used with 
single-die (3V) emitters. This driver features the Attiny1616 MCU, and comes
with on-board 10W intelligent battery charging.

## Additional Information

**Flashing firmware**: Rev B of the Lume1-6AF uses the Attiny1634 MCU, and
adopts the 6-pin flashing pads standard. Rev C & D of the Lume1-6AF, and the
Lume-X1 drivers use the Attiny1616 MCU with a 3-pin UPDI flashing interface. 
Pin-outs are printed on the PCB silkscreen.

**More information**: For more information on the Lume1-6AF driver, please 
see here: 
[https://github.com/loneoceans/lume1-ff-6af](https://github.com/loneoceans/lume1-ff-6af)

