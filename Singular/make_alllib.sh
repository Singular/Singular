#!/bin/sh
cp $1 LIB/all.lib.n
shift
for i in $*
do
	echo "LIB\"$i\";" >>LIB/all.lib.n
done
mv LIB/all.lib.n LIB/all.lib
