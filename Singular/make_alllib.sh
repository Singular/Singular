#!/bin/sh
cp $1 LIB/all.lib.n
chmod u+rw LIB/all.lib.n
shift
for i in $*
do
	echo "LIB \"$i\";" >>LIB/all.lib.n
done
chmod u-w LIB/all.lib.n
mv LIB/all.lib.n LIB/all.lib
