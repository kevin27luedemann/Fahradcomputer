TARGET=main
MCU=atmega1284p
SOURCES=$(TARGET).cpp SOUND.cpp BMP180.cpp I2C.cpp Display.cpp Interface.cpp Kompass.cpp LSM303D.cpp Output.cpp RTC.cpp diskio.c ff.c
#SOURCES=$(shell find . -type f -name '*.cpp')

PROGRAMMER=stk500
#auskommentieren für automatische Wahl
PORT=-P/dev/ttyUSB0
BAUD=-B11600

#Ab hier nichts verändern
OBJECTS=$(SOURCES:.cpp=.o)
CFLAGS=-c -O
LDFLAGS=-lprintf_flt -lm -fpermissive -u vfprintf 

all: hex eeprom
run: hex eeprom

hex: $(TARGET).hex

eeprom: $(TARGET)_eeprom.hex

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -j .data -j .text $(TARGET).elf $(TARGET).hex

$(TARGET)_eeprom.hex: $(TARGET).elf
	avr-objcopy -O ihex -j .eeprom --change-section-lma .eeprom=1 $(TARGET).elf $(TARGET)_eeprom.hex

$(TARGET).elf: $(OBJECTS) Monitor.h
	avr-g++ $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $(TARGET).elf

%.o : %.cpp
	avr-g++ $(CFLAGS) -mmcu=$(MCU) $< -o $@

size:
	avr-size --mcu=$(MCU) -C $(TARGET).elf

program: $(TARGET).hex
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Uflash:w:$(TARGET).hex:a

readee:
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Ueeprom:r:$(TARGET).eep:i

readfl:
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Uflash:r:$(TARGET).fla:i

clean_tmp:
	rm -rf *.o
	rm -rf *.elf

clean:
	rm -rf *.o
	rm -rf *.elf
	rm -rf *.hex
