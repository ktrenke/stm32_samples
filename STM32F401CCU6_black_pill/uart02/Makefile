
ARMGNU = arm-none-eabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m4
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding  -mcpu=cortex-m4

gcc : notmain.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.opt.s
	rm -f *.norm.s

#---------------------------------

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

notmain.o : notmain.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c notmain.c -o notmain.o

notmain.thumb2.o : notmain.c
	#$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m4 -march=armv7-m -c notmain.c -o notmain.thumb2.o
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m4 -c notmain.c -o notmain.thumb2.o

notmain.bin : flash.ld flash.o notmain.o
	$(ARMGNU)-ld -o notmain.elf -T flash.ld flash.o notmain.o
	$(ARMGNU)-objdump -D notmain.elf > notmain.list
	$(ARMGNU)-objcopy notmain.elf notmain.bin -O binary

