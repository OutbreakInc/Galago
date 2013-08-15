#!/bin/sh

if [ ! -f "module.json" ]
then
	echo "Error: You cannot prepare this directory for upload because there's no module.json file. Is it a Moduleverse module?"
	exit -1;
fi

mkdir -p .lpm
find . | grep -v -E "/\.|\./prepare-module\.sh" | cpio -o --format ustar | gzip -8 > .lpm/build.tar.gz

echo "Uploading module..."
curl -v --user logiblock -X PUT --upload-file .lpm/build.tar.gz http://moduleverse.com/v1/modules
