FILENAME = main
FILENAME2 = i2cmaster
FILENAME3 = uart
DEVICE = atmega328p
COMPILE = avr-gcc -Wall -Os -mmcu=$(DEVICE)
PROGRAMMER = arduino
PORT = /dev/ttyUSB0
BAUD = 115200

compile:
	$(COMPILE) -c $(FILENAME).c -o $(FILENAME).o
	$(COMPILE) -c $(FILENAME2).c -o $(FILENAME2).o
	$(COMPILE) -c $(FILENAME3).c -o $(FILENAME3).o
	$(COMPILE) -o $(FILENAME).elf $(FILENAME).o $(FILENAME2).o $(FILENAME3).o
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf

upload:
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(FILENAME).hex:i

clean:
	rm $(FILENAME).o
	rm $(FILENAME).elf
	rm $(FILENAME).hex
	rm $(FILENAME2).o
	rm $(FILENAME3).o
