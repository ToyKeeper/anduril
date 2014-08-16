PROGRAM = DarkHorse
MCU = attiny13
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS += -Wall -g -Os -mmcu=$(MCU)
LDFLAGS +=
OBJS = $(PROGRAM).o
# uncomment to remove raw commands from build output
#Q := @

all: $(PROGRAM).hex

$(PROGRAM).elf: $(PROGRAM).o
	@printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(PROGRAM).hex: $(PROGRAM).elf
	@printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O ihex $< $@
	@printf "  SIZE $(subst $(shell pwd)/,,$(@))\n"
	$(Q)avr-size $@

%.o: %.c
	@printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

flash: $(PROGRAM).hex
	@printf "  FLASH   $(PROGRAM).hex\n"
	$(Q)avrdude -c usbasp -p t13 -V -u -Uflash:w:$(PROGRAM).hex -Ulfuse:w:0x75:m -Uhfuse:w:0xFF:m

flash-example: precompiled.hex
	@printf "  FLASH   precompiled.hex\n"
	$(Q)avrdude -c usbasp -p t13 -V -u -Uflash:w:precompiled.hex -Ulfuse:w:0x75:m -Uhfuse:w:0xFF:m

clean:
	@printf "  CLEAN   $(subst $(shell pwd)/,,$(OBJS))\n"
	$(Q)rm -f $(OBJS)
	@printf "  CLEAN   $(PROGRAM).elf\n"
	$(Q)rm -f *.elf
	@printf "  CLEAN   $(PROGRAM).hex\n"
	$(Q)rm -f $(PROGRAM).hex
