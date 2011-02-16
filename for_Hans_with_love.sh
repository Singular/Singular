#!/bin/sh

autotools (){
aclocal
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
  aclocal
  autoconf
  automake -a
  cd $TOP_DIR
  echo
done

for d in omalloc libpolys/misc libpolys/resources libpolys/reporter libpolys/coeffs libpolys/polys; do
  echo "==============="
  echo "run aclocal, autoheader, autoconf in $d"
  echo "---------------"
  cd $d;
  autotools
  cd $TOP_DIR
  echo
done

