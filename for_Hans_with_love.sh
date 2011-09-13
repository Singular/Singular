#!/bin/sh

autotools (){
autoheader
autoconf
automake -a
}

TOP_DIR=$PWD

for d in . libpolys; do
  echo "==============="
  echo "run libtoolize --force, aclocal -I $TOP_DIR/m4, autoconf, automake -a in $d"
  echo "---------------"
  cd $d;
  libtoolize --force
  aclocal -I $TOP_DIR/m4
  autotools
  cd $TOP_DIR
  echo
done

for d in omalloc factory; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4, autotools in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autotools
  cd $TOP_DIR
  echo
done
