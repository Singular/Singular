#!/bin/sh
#C="$1"
#C=`readlink -f "$C"`
#C=`dirname "$C"`
#C=`ls -d1 "$C"`

cp $1 all.lib.n
chmod u+rw all.lib.n
shift
for i in $*
do
	echo "LIB \"$i\";" >> all.lib.n
done
#	[ ! -e "$i" ] && (cp -nv "$C/$i" .)
chmod u-w all.lib.n
mv -f all.lib.n all.lib
