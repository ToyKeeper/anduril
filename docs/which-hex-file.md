# How to figure out which .hex file to use

1. Use the version check function (15 or more clicks from Off) to find out
   which flavor of the firmware the light has installed.  
   **WRITE THIS DOWN.**

2. Look up the model number in the MODELS file, to get the name and MCU type:  
   https://toykeeper.net/torches/fsm/anduril2/MODELS

3. Find the newest firmware for that model.  
   https://toykeeper.net/torches/fsm/anduril2/?C=M;O=D

If the version doesn't include a model number, you may be able to find
the model in the PRODUCTS file to see which version it probably uses:

  https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/view/head:/PRODUCTS


## Version Check Formats

The Version Check function should blink out a series of numbers in one of
several formats:

 - `NNNN-YYYY-MM-DD`
   Anduril 2 from 2023-05 or later.  
   It's a model number and build date,
   with "buzz" flashes between sections.
   - `NNNN`: model number
   - `YYYY`: year
   - `MM`: month
   - `DD`: day

 - `YYYYMMDDNNNN`
   Anduril 2 from 2023-05 or earlier.  
   It's a build date and model number.

- `YYYYMMDD`
   This is an old Anduril 1 version.  It's just a build date.  
   If the model name isn't obvious, try looking it up in the PRODUCTS file.

