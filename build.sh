#!/bin/sh

markup=`find * | grep -e "\.hbs\.html$" | grep -ve "^template/|^node_modules/"`

for m in $markup
do
	out=`echo $m | sed 's/\.hbs//g'`
	cat template/header.hbs.html $m template/footer.hbs.html > $out
done