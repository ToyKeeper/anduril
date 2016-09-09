#/bin/sh
FIRMWARE=$1
avrdude -c usbasp -p t25 -u -U lfuse:w:0xd2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -Uflash:w:$FIRMWARE
# http://www.engbedded.com/cgi-bin/fcx.cgi?P_PREV=ATtiny25&P=ATtiny25&M_LOW_0x3F=0x22&M_HIGH_0x07=0x07&M_HIGH_0x20=0x00&B_SUT1=P&B_SPIEN=P&B_CKSEL3=P&B_CKSEL2=P&B_CKSEL0=P&V_LOW=D2&V_HIGH=DF&V_EXTENDED=FF
