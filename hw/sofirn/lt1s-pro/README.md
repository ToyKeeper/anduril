# Sofirn LT1S Pro

21700 lantern with 3 individual LED channels (neutral white, warm white, and red).

Built in USB-C charging and LED battery indicator. The LED battery indicator is not controlled by Anduril and will activate whenever the switch is pressed, and has four LEDs - all four lit corresponds to around 4.1-4.2V, and one lit corresponds to around 3.7V or less. When charging using the USB port, the LEDs will animate in a "charging" pattern.

## Channel Modes
### White
AKA `CM_WHITE`. This is a ramping channel mode with a manually adjustable mix between the 2700k cool white LEDs and 5000k neutral white.

### Auto-Tint (White)
AKA `CM_AUTO2`. This is an auto-tint mode where the mix will shift from 2700k at low ramp to 5000k at maximum level.

### Auto-Tint (White/Red)
AKA `CM_AUTO3`. This is an auto-tint mode where the mix will shift from red at the bottom of the ramp, then to white, initially at 2700k before then increasing to 5000k at maximum level.

### Red
AKA `CM_RED`. Only uses the red LEDs.

### White/Red
AKA `CM_WHITE_RED`. This is a ramping channel mode with a manually adjustable mix between white and red. The mix of the two white channels in this mode is the same as the selected mix in the white (`CM_WHITE`) channel.

