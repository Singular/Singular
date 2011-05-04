#!/bin/sh

autotools (){
autoheader
autoconf
automake -a
}

TOP_DIR=$PWD

for d in .; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4, autoconf, automake -a in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autoconf
  automake -a
  cd $TOP_DIR
  echo
done

for d in omalloc; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4, autotools in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autotools
  cd $TOP_DIR
  echo
done

for d in factory; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4, autoconf in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autoconf
  cd $TOP_DIR
  echo
done

for d in libpolys; do
  echo "==============="
  echo "run libtoolize --force, aclocal -I $TOP_DIR/m4, autotools in $d"
  echo "---------------"
  cd $d;
  libtoolize --force
  aclocal -I $TOP_DIR/m4
  autotools
  cd $TOP_DIR
  echo
done

