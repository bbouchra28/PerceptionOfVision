EXEC=main

all: $(EXEC).bin

$(EXEC).elf: $(EXEC).c
	avr-gcc -mmcu=atmega128 -Os -DF_CPU=13000000 $(EXEC).c -o $@ -Wl,-u,vfprintf -lprintf_flt -lm

$(EXEC).bin: $(EXEC).elf
	avr-objcopy -O binary $< $@


clean:
	rm *.bin *.elf

install: $(EXEC).bin
	avrdude -p m128 -c jtagmkI -P /dev/ttyUSB0 -U flash:w:$(EXEC).bin:r
