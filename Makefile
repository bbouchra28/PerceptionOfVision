EXEC=main

all: $(EXEC).bin

usart.o: usart.c
	avr-gcc -Wall -Os -mmcu=atmega128 -DF_CPU=13000000 -c $<

led.o: led.c
	avr-gcc -Wall -Os -mmcu=atmega128 -DF_CPU=13000000 -c $<

text.o: text.c
	avr-gcc -Wall -Os -mmcu=atmega128 -DF_CPU=13000000 -c $<

main.o: main.c
	avr-gcc -Wall -Os -mmcu=atmega128 -DF_CPU=13000000 -c $<

$(EXEC).elf: $(EXEC).o usart.o led.o text.o
	avr-gcc -mmcu=atmega128 -o $@ $^ -Wl,-u,vfprintf -lprintf_flt -lm
	#avr-gcc -mmcu=atmega128 -Os -DF_CPU=13000000 $(EXEC).c -o $@ -Wl,-u,vfprintf -lprintf_flt -lm

$(EXEC).bin: $(EXEC).elf
	avr-objcopy -O binary $< $@


clean:
	rm *.bin *.elf *.o

install: $(EXEC).bin
	avrdude -p m128 -c jtagmkI -P /dev/ttyUSB0 -U flash:w:$(EXEC).bin:r
