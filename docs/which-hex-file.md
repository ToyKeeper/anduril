# How to figure out which .hex file to use

Using the wrong firmware will make the light stop working, so when flashing
firmware, **be sure the model number matches**!

The name of a product is *not* enough information to find the right .hex
file.  Ask the light what firmware it needs:

1. Use the Version Check function (15 or more clicks from Off) to find out
   which flavor of the firmware the light has installed.  
   **WRITE THIS DOWN.**

2. Look up the model number in the [MODELS file](../MODELS),
   to get the name and MCU type.

3. Find (or build) the newest firmware for that model.  
   https://github.com/ToyKeeper/anduril/releases

The Version Check format has changed a few times.  Use
[this section of the manual](anduril-manual.md#Version_Check_Formats)
to learn about those formats and how to use them.  In most cases, it should
have a model number and a date... and **you need the model number** to find
the correct .hex file.

