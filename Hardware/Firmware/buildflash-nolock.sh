#!/bin/sh

if [ ! -f "build/$1/uuidOffset.txt" ] || [ ! -f "build/$1/$1.bin" ]
then
	echo "Error, build/$1/$1.bin is not valid firmware"
	exit -1
fi
node serialize.js `cat build/$1/uuidOffset.txt` < build/$1/$1.bin > out.bin
avrdude -u -P usb -c avrisp2 -p t45 -Uflash:w:out.bin:r -Ulfuse:w:0xe1:m
hexdump -s 0x`cat build/$1/uuidOffset.txt` -n 16 -e '"UUID: "' -e '8/2 "%04x "' -e '"\n"' out.bin
