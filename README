ToyKeeper's flashlight firmware repository
==========================================

This is a collection of firmware for flashlights or torches, mostly 
collected from BLF (http://budgetlightforum.com/).  It's collected 
here mostly for convenience, and to provide bug tracking and revision 
control services for the code.

See the INDEX file to find out which projects might be relevant to 
your needs.


Getting Started
===============

The central firmware thread on BLF is here:

  http://budgetlightforum.com/node/38364

A general overview of what you need to get going is here:

  http://flashlightwiki.com/AVR_Drivers

Some useful hardware for flashing firmware:

  What TK uses:

    http://www.fasttech.com/product/1002900-atmega-attiny-51-avr-isp-usbasp-usb-programmer
    http://www.fasttech.com/product/1011800-40-pin-splittable-ribbon-cable-20cm
    http://www.digikey.com/product-detail/en/5250/501-1311-ND/745102
    HQ ProgKey: http://budgetlightforum.com/node/63230

  Ratus' guide:  https://redd.it/8g5l5w  (prices checked 2018-05-01)

    Store A: About $4.20 shipped
      https://www.aliexpress.com/item/USB-ISP-Programmer-for-ATMEL-AVR-ATMega-ATTiny-51-AVR-Board-ISP-Downloader/32699341177.html
      https://www.aliexpress.com/item/WAVGAT-Programmer-Testing-Clip-SOP8-SOP-SOIC-8-SOIC8-DIP8-DIP-8-Pin-IC-Test-Clamp/32827794024.html
      https://www.aliexpress.com/item/40Pin-20CM-2-54MM-Row-Female-to-Female-F-F-Dupont-Cable-Breadboard-Jumper-Wire-for/32822958653.html

    Store B: About $4.18 shipped:
      https://www.aliexpress.com/store/product/1pcs-New-USBASP-USBISP-AVR-Programmer-USB-ISP-USB-ASP-ATMEGA8-ATMEGA128-Support-Win7-64K/1171090_32809542958.html
      https://www.aliexpress.com/store/product/Free-shipping-Programmer-Testing-Clip-SOP8-SOP-SOIC-8-SOIC8-DIP8-DIP-8-Pin-IC-Test/1171090_32402561848.html
      https://www.aliexpress.com/store/product/20pcs-20cm-2-54mm-1p-1p-Pin-Female-to-Female-Color-Breadboard-Cable-Jump-Wire-Jumper/1171090_32628811461.html

For Windows / MacOS users, these links may be useful:

  Oweban's guides:  (Windows 10, MacOS X) (not generic, focuses on Anduril)

    http://budgetlightforum.com/node/73487

  Hoop's guide:

    http://budgetlightforum.com/node/36216

  Comfychair's guide:

    http://budgetlightforum.com/node/29081

  WarHawk-AVG's guide:

    http://budgetlightforum.com/node/29684

  Getting a USBasp to work in Windows:

    http://rayshobby.net/dead-simple-driver-installation-for-usbasp-and-usbtiny-on-windows/
    http://www.protostack.com/accessories/usbasp-avr-programmer

To set up an attiny dev environment on Ubuntu (13.10):

  sudo apt-get install flex byacc bison gcc libusb-dev libc6-dev
  sudo apt-get install gcc-avr avr-libc binutils-avr
  sudo apt-get install avrdude avrdude-doc

  Optional:  (make avrdude usable by non-root users, is a security risk)
  sudo chmod u+s $(which avrdude)

To set up an attiny dev environment on Fedora (30):

  sudo dnf install flex byacc bison gcc libusb libusb-devel glibc-devel
  sudo dnf install avr-gcc avr-libc avr-binutils
  sudo dnf install avrdude

Building/installing attiny dev tools on other UNIX systems (in general):

  http://www.ladyada.net/learn/avr/setup-unix.html

After wiring everything up, this tests the connection to the ATTINY13A chip:

  avrdude -p t13 -c usbasp -n
  (you may need to put 'sudo' in front, on UNIX systems...
  or set the suid bit on the avrdude binary, which is convenient but a
  potential security risk)

To test if you can flash firmware correctly, or to restore a light to something
like a default state, try flashing the NLITE driver from DrJones.  It's similar
to the default nanjg / qlite firmware, but better.

  ./bin/flash-tiny13.sh DrJones/NLITE/nlite.hex

For a quick test to make sure your tool chain works and your flashing tools
are working, try the 'hello world' program:

  cd hello_world
  make
  make flash

If that works, you should be ready to start making changes to the code, or
flashing any of the pre-built firmwares.

Most projects can be compiled with the bin/build.sh script.  For example, to
build Crescendo for an attiny25-based driver and then flash it:

  cd ToyKeeper/crescendo
  ../../bin/build.sh 25 crescendo
  ../../bin/flash-tiny25.sh crescendo.hex

Other useful tools are in bin/ too, so they might be worth a look.


ATtiny Series 1 (tiny1616) Support
==================================

In Feb 2022, avr-libc merged in new devices from Atmel's DFP (Device Family
Pack).  Depending on how recent your avr-libc package is, support may already
be included.  If not, the info below applies:

As of early 2021, the Debian packages for gcc-avr and avrdude do not include
support for the Tiny1 series chips.  Extra steps are required to get these
working.

  - Download the Atmel ATtiny Series Device Support pack:
    http://packs.download.atmel.com/

  - Unzip the pack somewhere on your build computer

  - Set ATTINY_DFP=/path/to/where/you/unzipped/the/pack
    (either in your shell, or in this repo's bin/build.sh script)

    export ATTINY_DFP=$HOME/src/torches/atmel/attiny-dfp

  - Make sure you're using gcc-avr 1:5.4.0+Atmel3.6.2 or newer.
    3.6.1 will not work.  It gives errors like:
    /usr/lib/gcc/avr/5.4.0/../../../avr/bin/ld: address 0x80381e of anduril.elf section `.data' is not within region `data'
    This requires debian/bullseye or newer; buster has 3.6.1.

This should at least allow the code to compile.

Flashing:

PyMCUProg

The recommended solution is to use pymcuprog instead of avrdude.  Details are
still being worked out for how to actually set this up though.

AvrDude

Some extra steps are also needed to make flashing (avrdude) work:

  - Get an AVR jtag2 device.
    The one I'm using is: "HWAYEH AVR JTAG ICE Version 2.0"
    Connect the cables:
      - GND   = -
      - Vtref = +
      - nSRST = R (reset/UPDI)

  - Flash the jtag2 device with the relevant firmware:
    https://github.com/ElTangas/jtag2updi/tree/master/tools/avrjtagicev2

  - Get an avrdude.conf which supports jtag2updi:

    - Download avrdude.conf from https://github.com/ElTangas/jtag2updi
      and put it in /etc

    - Or grab the source and set an environment variable:

      cd ~/src/torches/avrdude
      git clone https://github.com/ElTangas/jtag2updi
      export AVRDUDE_CONF="-C$HOME/src/torches/avrdude/jtag2updi/avrdude.conf"

  - Maybe configure which USB serial port to use too:
    export AVRDUDE_TTYUSB="/dev/ttyUSB2"

Afterward, flashing should work.  You may need to unplug and replug the jtag2
USB device between uses though, since it may stop responding after each use.
