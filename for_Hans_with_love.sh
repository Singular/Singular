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

for d in . libpolys; do
  echo "==============="
  echo "run aclocal, autoconf in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autoconf
  automake -a
  cd $TOP_DIR
  echo
done

for d in omalloc libpolys/misc libpolys/resources libpolys/reporter libpolys/coeffs; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4, autoheader, autoconf in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4
  autotools
  cd $TOP_DIR
  echo
done

for d in libpolys/polys; do
  echo "==============="
  echo "run aclocal -I $TOP_DIR/m4 -I m4, autoheader, autoconf in $d"
  echo "---------------"
  cd $d;
  aclocal -I $TOP_DIR/m4 -I m4
  autotools
  cd $TOP_DIR
  echo
done

