avrdude -p t13 -c usbasp -u -Uflash:w:%1.hex:a -Ulfuse:w:0x75:m -Uhfuse:w:0xFF:m
