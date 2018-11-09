#/bin/sh
# 4.8 MHz (~4.0 MHz actual), 4ms boot delay, enable flashing
# (everything else disabled)
# Use low fuse 0x75 for 4ms startup delay,
#           or 0x79 for 64ms (useful on a twisty light)
avrdude -c usbasp -p t13 -u -Ulfuse:w:0x75:m -Uhfuse:w:0xFF:m

# http://www.engbedded.com/cgi-bin/fcx.cgi?P_PREV=ATtiny13A&P=ATtiny13A&M_LOW_0x0F=0x05&M_LOW_0x80=0x00&M_HIGH_0x06=0x06&B_SPIEN=P&B_SUT1=P&B_CKSEL1=P&V_LOW=75&V_HIGH=FF&O_HEX=Apply+values
