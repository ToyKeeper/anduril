#/bin/sh
FIRMWARE=$1
avrdude -c usbasp -p t85 -u -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -Uflash:w:$FIRMWARE
# http://www.engbedded.com/cgi-bin/fcx.cgi?P_PREV=ATtiny85&P=ATtiny85&M_LOW_0x3F=0x22&M_HIGH_0x07=0x06&M_HIGH_0x20=0x00&B_SPIEN=P&B_SUT0=P&B_CKSEL3=P&B_CKSEL2=P&B_CKSEL0=P&B_BODLEVEL0=P&V_LOW=E2&V_HIGH=DE&V_EXTENDED=FF
