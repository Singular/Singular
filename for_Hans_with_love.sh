#!/bin/sh

autotools (){
aclocal
autoheader
autoconf
automake -a
}

TOP_DIR=$PWD

for d in . omalloc libpolys/ libpolys/misc libpolys/resources libpolys/reporter libpolys/coeffs libpolys/polys; do
  echo "==============="
  echo "run aclocal, autoheader, autoconf in $d"
  echo "---------------"
  cd $d;
  autotools
  cd $TOP_DIR
  echo
done

