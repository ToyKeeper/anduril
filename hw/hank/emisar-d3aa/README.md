# Emisar D3AA

14500/AA single channel light with HDR boost driver by [thefreeman](https://budgetlightforum.com/u/thefreeman). thefreeman has done an [excellent technical writeup on this driver](https://budgetlightforum.com/t/emisar-d3aa-driver-technical-information/223513).

No built in charging.

## Weak Battery Protection
When the D3AA boots (each time power is connected) it will test the battery's performance and set a hard cap on output, to prevent damage to weak batteries, as the driver is capable of drawing more current than is safe for the battery from alkaline and Ni-MH batteries. This may also cause reduced turbo output if a very low li-ion battery is inserted and the weak battery protection is activated. This will not happen if a 14500 battery is inserted at a high charge level and then used until charge drops, unless the tailcap is untwisted and reconnected.

## Channel Modes
### Main LEDs
AKA `CM_MAIN`. The three main LEDs, driven in series as a single channel.

### RGB Aux Modes
These channel modes (in order: red, orange/yellow, green, cyan, blue, purple, white) use aux instead of main emitters. This is useful for using red or white as a vision-preserving mode in extremely low light situations, otherwise the main use of these channels is to be selected for strobe or blinky modes.

