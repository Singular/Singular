#! /bin/sh

cd `dirname "$0"`

# -d --warnings=all
autoreconf  -v -f -i --include=`pwd`/m4

cd -

