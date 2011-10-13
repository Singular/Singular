#!/bin/sh

autoreconf -vifs

# TOP_DIR=$PWD
# 
# autotools ()
# {
#   aclocal -I $TOP_DIR/m4
#   autoheader
#   autoconf
#   automake -a
# }
# 
# 
# for d in . factory libpolys; do
#   echo "==============="
#   echo "run libtoolize --force, aclocal -I $TOP_DIR/m4, autoconf, automake -a in $d"
#   echo "---------------"
#   cd $d
#   libtoolize --force
#   autotools
#   cd $TOP_DIR
#   echo
# done
# 
# for d in omalloc; do
#   echo "==============="
#   echo "run aclocal -I $TOP_DIR/m4, autotools in $d"
#   echo "---------------"
#   cd $d
#   autotools
#   cd $TOP_DIR
#   echo
# done
