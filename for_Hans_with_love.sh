#!/bin/sh

echo "==============="
echo "run libtoolize in libpolys/polys/"
echo "---------------"
( cd libpolys/polys/; libtoolize; )

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

for d in libpolys; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4, autotools in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autotools
  cd $TOP_DIR
  echo
done

for d in omalloc libpolys/coeffs; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4, autotools in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autotools
  cd $TOP_DIR
  echo
done

for d in libpolys/polys; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4 -I m4, autotools in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4 -I m4
  autotools
  cd $TOP_DIR
  echo
done

