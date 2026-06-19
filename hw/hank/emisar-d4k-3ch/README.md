# Emisar D4K Triple Channel

21700 light with 3 individual LED channels. The first channel uses LEDs 1+2, the second LED 3, and the third LED 4.

No built in charging.

## Channel Modes

The D4K Triple Channel has two separate wiring possibilities. There are three separate PWM channels available on the MCU, one of these is 8-bit (lower ramp resolution) and the other two are 16-bit. In addition, delta-sigma modulation is used to improve the ramp resolution of the 8-bit channel.

The more common wiring will use the 8-bit channel for the main two LEDs, and a 16-bit channel for each single LED channel. The alternate wiring arrangement uses a 16-bit PWM counter for the main two LEDs and the third LED, and the 12-bit for the 4th LED.

### Channel 1
AKA `CM_MAIN2`. On the standard wiring pattern, this is the 2-LED "main" channel. On the alternate wiring pattern, this is the 4th LED (third channel).

### Channel 2
AKA `CM_LED3`. On both the standard and alternate wiring patterns, this is the third LED (second channel).

### Channel 3
AKA `CM_LED4`. On the standard wiring pattern, this is the 4th LED (third channel). On the alternate wiring pattern, this is the 2-LED "main" channel.

### All
AKA `CM_ALL`. On both wiring patterns, this is all three main channels with an even mix, and "300%" power at ceiling level.

### Channel 3+4
AKA `CM_BLEND34A`. Ramping mode between two channels - on the standard wiring pattern, this is the 3rd and 4th LEDs (channels 2 and 3). On the alternate wiring pattern, this is the 4th and main two LEDs (channels 1 and 4).

### Channel 1+2
AKA `CM_BLEND34B`. Ramping mode between two channels - on the standard wiring pattern, this is the main two and 3rd LEDs (channels 1 and 2). On the alternate wiring pattern, this is the 3rd and 4th LEDs (channels 2 and 3).

### HSV
AKA `CM_HSV`. Circular ramp between all three channels.

### Auto-tint
AKA `CM_AUTO3`. Auto-tint based on ramp level. On the standard wiring pattern, this starts at floor level with channel 3 (LED 4), fading into channel 2 (LED 3) as ramp level increased, followed by channel 1 (main two LEDs). On the alternate wiring the main two LEDs and channel 3 are reversed

### RGB Aux Modes
These channel modes (in order: red, orange/yellow, green, cyan, blue, purple, white) use aux instead of main emitters. This is useful for using red or white as a vision-preserving mode in extremely low light situations, otherwise the main use of these channels is to be selected for strobe or blinky modes.

