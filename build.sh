#!/bin/sh

echo Building in `pwd`...

markup=`find * | grep -e "\.hbs\.html$" | grep -ve "^template/"`
for m in $markup
do
	out=`echo $m | sed 's/\.hbs//g'`
	cat template/header.hbs.html $m template/footer.hbs.html > $out
	echo : built $out
	git add $out
done

less=`find * | grep -e "\.less\.css$"`
for css in $less
do
	out=`echo $css | sed 's/\.less//g'`
	node_modules/.bin/lessc -x $css $out
	echo : built $out
	git add $out
done

echo Build complete
