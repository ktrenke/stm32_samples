
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m0
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding  -mcpu=cortex-m0

LOPS = -Wall -m32 -emit-llvm -target arm-none-eabi -mcpu=cortex-m0 -mthumb
LLCOPS = -march=thumb -mcpu=cortex-m0
#LLCOPS = -mcpu=cortex-m0
COPS = -Wall  -O2 -nostdlib -nostartfiles -ffreestanding
#at some point they changed this option, gee thanks
#OOPS = -std-compile-opts
OOPS = -std-link-opts

gcc : blinker07.gcc.thumb.flash.bin blinker07.gcc.thumb.sram.bin

all : gcc clang

clang : blinker07.clang.thumb.norm.flash.bin blinker07.clang.thumb.opt.flash.bin blinker07.clang.thumb.norm.sram.bin blinker07.clang.thumb.opt.sram.bin 

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.opt.s
	rm -f *.norm.s
	rm -f *.hex

#---------------------------------

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

sram.o : sram.s
	$(ARMGNU)-as $(AOPS) sram.s -o sram.o

blinker07.gcc.thumb.o : blinker07.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c blinker07.c -o blinker07.gcc.thumb.o

blinker07.gcc.thumb.flash.bin : flash.ld flash.o blinker07.gcc.thumb.o
	$(ARMGNU)-ld -o blinker07.gcc.thumb.flash.elf -T flash.ld flash.o blinker07.gcc.thumb.o
	$(ARMGNU)-objdump -D blinker07.gcc.thumb.flash.elf > blinker07.gcc.thumb.flash.list
	$(ARMGNU)-objcopy blinker07.gcc.thumb.flash.elf blinker07.gcc.thumb.flash.bin -O binary

blinker07.gcc.thumb.sram.bin : sram.ld sram.o blinker07.gcc.thumb.o
	$(ARMGNU)-ld -o blinker07.gcc.thumb.sram.elf -T sram.ld sram.o blinker07.gcc.thumb.o
	$(ARMGNU)-objdump -D blinker07.gcc.thumb.sram.elf > blinker07.gcc.thumb.sram.list
	$(ARMGNU)-objcopy blinker07.gcc.thumb.sram.elf blinker07.gcc.thumb.sram.hex -O ihex
	$(ARMGNU)-objcopy blinker07.gcc.thumb.sram.elf blinker07.gcc.thumb.sram.bin -O binary

#---------------------------------

blinker07.clang.bc : blinker07.c
	clang $(LOPS) -c blinker07.c -o blinker07.clang.bc

blinker07.clang.thumb.norm.flash.bin : flash.ld flash.o blinker07.clang.bc
	#llc $(LLCOPS) blinker07.clang.bc -o blinker07.clang.thumb.norm.s
	#$(ARMGNU)-as $(AOPS) blinker07.clang.thumb.norm.s -o blinker07.clang.thumb.norm.o
	llc $(LLCOPS) blinker07.clang.bc -filetype=obj -o blinker07.clang.thumb.norm.o
	$(ARMGNU)-ld -o blinker07.clang.thumb.norm.flash.elf -T flash.ld flash.o blinker07.clang.thumb.norm.o
	$(ARMGNU)-objdump -D blinker07.clang.thumb.norm.flash.elf > blinker07.clang.thumb.norm.flash.list
	$(ARMGNU)-objcopy blinker07.clang.thumb.norm.flash.elf blinker07.clang.thumb.norm.flash.bin -O binary

blinker07.clang.thumb.opt.flash.bin : flash.ld flash.o blinker07.clang.bc
	opt $(OOPS) blinker07.clang.bc -o blinker07.clang.thumb.opt.bc
	#llc $(LLCOPS) blinker07.clang.thumb.opt.bc -o blinker07.clang.thumb.opt.s
	#$(ARMGNU)-as $(AOPS) blinker07.clang.thumb.opt.s -o blinker07.clang.thumb.opt.o
	llc $(LLCOPS) blinker07.clang.thumb.opt.bc -filetype=obj -o blinker07.clang.thumb.opt.o
	$(ARMGNU)-ld -o blinker07.clang.thumb.opt.flash.elf -T flash.ld flash.o blinker07.clang.thumb.opt.o
	$(ARMGNU)-objdump -D blinker07.clang.thumb.opt.flash.elf > blinker07.clang.thumb.opt.flash.list
	$(ARMGNU)-objcopy blinker07.clang.thumb.opt.flash.elf blinker07.clang.thumb.opt.flash.bin -O binary


blinker07.clang.thumb.norm.sram.bin : sram.ld sram.o blinker07.clang.bc
	#llc $(LLCOPS) blinker07.clang.bc -o blinker07.clang.thumb.norm.s
	#$(ARMGNU)-as $(AOPS) blinker07.clang.thumb.norm.s -o blinker07.clang.thumb.norm.o
	llc $(LLCOPS) blinker07.clang.bc -filetype=obj -o blinker07.clang.thumb.norm.o
	$(ARMGNU)-ld -o blinker07.clang.thumb.norm.sram.elf -T sram.ld sram.o blinker07.clang.thumb.norm.o
	$(ARMGNU)-objdump -D blinker07.clang.thumb.norm.sram.elf > blinker07.clang.thumb.norm.sram.list
	$(ARMGNU)-objcopy blinker07.clang.thumb.norm.sram.elf blinker07.clang.thumb.norm.sram.hex -O ihex
	$(ARMGNU)-objcopy blinker07.clang.thumb.norm.sram.elf blinker07.clang.thumb.norm.sram.bin -O binary

blinker07.clang.thumb.opt.sram.bin : sram.ld sram.o blinker07.clang.bc
	opt $(OOPS) blinker07.clang.bc -o blinker07.clang.thumb.opt.bc
	#llc $(LLCOPS) blinker07.clang.thumb.opt.bc -o blinker07.clang.thumb.opt.s
	#$(ARMGNU)-as $(AOPS) blinker07.clang.thumb.opt.s -o blinker07.clang.thumb.opt.o
	llc $(LLCOPS) blinker07.clang.thumb.opt.bc -filetype=obj -o blinker07.clang.thumb.opt.o
	$(ARMGNU)-ld -o blinker07.clang.thumb.opt.sram.elf -T sram.ld sram.o blinker07.clang.thumb.opt.o
	$(ARMGNU)-objdump -D blinker07.clang.thumb.opt.sram.elf > blinker07.clang.thumb.opt.sram.list
	$(ARMGNU)-objcopy blinker07.clang.thumb.opt.sram.elf blinker07.clang.thumb.opt.sram.hex -O ihex
	$(ARMGNU)-objcopy blinker07.clang.thumb.opt.sram.elf blinker07.clang.thumb.opt.sram.bin -O binary




