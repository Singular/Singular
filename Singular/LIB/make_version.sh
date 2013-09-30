#!/bin/bash
if [ $# -lt 3 ] #test ob >=3 Argumente
then
  echo "Usage: `basename $0` version_name version_date libs"
  exit 1;
fi

v=$1 # version: 3-1-7-0
shift
d=$1 # date: Sep_2013
shift
for l in $*
do
  sed "s/^version=.*/version=\"version $l $v $d \";/" <$l >/tmp/make_version
  mv /tmp/make_version $l
done
