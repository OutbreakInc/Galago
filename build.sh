#!/bin/sh

echo Building in `pwd`...

markup=`find * | grep -e "\.hbs\.html$" | grep -ve "^template/"`
for m in $markup
do
	out=`echo $m | sed 's/\.hbs//g'`
	
	# read params from template
	t="template/header.hbs.html"
	params=`grep "{{.*}}" $t | sed -E 's/.*{{(.*)}}.*/\1/g'`
	
	temp=`mktemp -t hbs`
	cat template/header.hbs.html > $temp
	
	for p in $params
	do
		value=`grep "<!-- @meta.*$p" $m | sed -E 's/<!-- @meta (.*) -->/\1/g'`

		#echo $value
		if [ ! "$value" ]
		then
			value="{$p=\"\"}"
		fi
		
		re=`echo $value | sed -E 's/\{\s*(.*)\s*=\s*"(.*)"\s*\}/s\/\{\{\1\}\}\/\2\/g/'`

		#echo $re
		sed -iE "$re" $temp
	done

	cat $temp $m template/footer.hbs.html > $out
	rm $temp

	echo : built $out
	git add $out
done

echo Compiling less...
less=`find * | grep -e "\.less\.css$"`
for css in $less
do
	out=`echo $css | sed 's/\.less//g'`
	node_modules/.bin/lessc -x $css $out
	echo : built $out
	git add $out
done

echo Build complete

exit


	# markdownify
	grep -E "\|$" GalagoAPI.h | sed -E 's/^   (.*)\|$/\1/g' | sed -E 's/[ ]*$//g' | node_modules/.bin/md2html > learn/api/raw.html
