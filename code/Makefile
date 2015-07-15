export AVR_CC		= avr-gcc
AVR_LD			= $(AVR_CC)
AVR_OBJCOPY		= avr-objcopy
AVR_SIZE		= avr-size
MCU			= atmega32
export CFLAGS		= -c -Wunused -Wall -Wstrict-prototypes -Wundef -Werror -pedantic -g -$(OPTIMIZE) -mmcu=$(MCU) -funsigned-char -funsigned-bitfields -fpack-struct -fdata-sections -ffunction-sections -fdiagnostics-color=always -std=gnu99
# -mcall-prologues
OPTIMIZE		= O2
LDFLAGS			= -fdiagnostics-color=always -Wl,--gc-sections
TARGET			= main
SIZE_HISTORY		= size.log

export OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

.NOTPARALLEL:

all: $(TARGET).hex

$(TARGET): compile_objects
	$(AVR_LD) -o $@.elf $(OBJECTS) $(LDFLAGS) -mmcu=$(MCU)

.PHONY: compile_objects
compile_objects:
	$(MAKE) -f Makefile.compile


%.hex: $(TARGET) size plot
	$(AVR_OBJCOPY) -j .text -j .data -O ihex $<.elf $@

.PHONY: clean
clean:
	rm -rf *.o $(TARGET).elf $(TARGET).hex

.PHONY: size
size: $(TARGET)
	@$(AVR_SIZE) -C --mcu=$(MCU) $(TARGET).elf
	@$(AVR_SIZE) -B --mcu=$(MCU) $(TARGET).elf | tail -n 1 >> $(SIZE_HISTORY)

.PHONY: install
install: $(TARGET).hex reset
	avrdude -V -p m32 -c stk500v2 -b 115200 -P /dev/ttyUSB0 -U flash:w:$<

.PHONY: plot
plot:
	@echo "Plotting build size history..."
	@gnuplot plot 2>/dev/null

.PHONY: reset
reset:
	@echo "Resetting MCU..."
	@stty -F /dev/ttyUSB0 9600 cs8 cread clocal
	@echo "R" > /dev/ttyUSB0
	@sleep 0.1
