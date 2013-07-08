#!/bin/sh
# Shockingly basic build system, but 100x better than GNU make
#  why better? the average user can make a modification to this in one second
#  that would take > 100 seconds to do (and get working properly) in a makefile

product=`basename $1 | sed -E 's/(.*)?\.(c|cc|cpp|cxx)$/\1/'`
out="build/$product"
mkdir -p $out
avr-gcc -mmcu=$MCU -D$MCU_DEF -DF_CPU=$F_CPU -std=c99 -Os -o $out/$product.elf $1 AppController.c
if [ $? -eq 0 ]; then
	avr-objdump -d $out/$product.elf > $out/$product.disasm.txt
	if [ -f $out/$product.bin ]; then
		rm $out/$product.bin
	fi
	avr-objcopy -j .text -j .data -O binary $out/$product.elf $out/$product.bin
	avr-size $out/$product.elf
	
	avr-nm $out/$product.elf | grep kUUIDAndDescriptor | cut -d ' ' -f 1 > $out/uuidOffset.txt
fi
