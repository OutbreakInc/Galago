#!/usr/bin/env node

// Requires Node.js

// Run with no arguments to see usage.

var crypto = require("crypto");

var templateFirmwareChunks = [];
var uuidOffset;

process.stdin.on("data", function(data)
{
	templateFirmwareChunks.push(data);
});
process.stdin.on("end", function()
{
	var templateFirmware = Buffer.concat(templateFirmwareChunks);
	
	var b1 = templateFirmware.slice(0, uuidOffset);
	
	var b2 = templateFirmware.slice(uuidOffset + 16);
	
	crypto.randomBytes(16, function(error, uuid)
	{
		process.stdout.write(Buffer.concat([b1, uuid, b2]));
		process.exit(0);
	});
});

if((process.argv.length < 2) || !(uuidOffset = parseInt(process.argv[2], 16)))
{
	console.warn(
"usage: serialize.js <uuidOffset (hex)>\n\
\n\
 -  uuidOffset is the offset, in hex, where the 16-byte UUID data begins.\n\
    This can be dertermined from running `nm firmware.elf | grep kUUID`\n\
    and looks like e.g. 0000003ae.\n\
\n\
 -  Input the template firmware on stdin and receive the serialized firmware\n\
    on stdout:\n\
    \n\
    node serialize.js `cat uuidOffset.txt` < firmware.bin > output.bin\n\
    \n\
    Remember to pass in a raw firmware binary, not an ELF file.  The output will\n\
    also be binary firmware.");
	process.exit(-1);
}
process.stdin.resume();
