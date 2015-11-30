#!/bin/sh
VERSION=4.0.2p3
export VERSION

git archive --prefix=singular-$VERSION/ HEAD |tar xf -
mkdir singular-$VERSION/doc
cp doc/*.man singular-$VERSION/doc/.
if test -e doc/doc.tbz2
then
  cp doc/doc.tbz2 singular-$VERSION/doc/.
fi

command rm singular-$VERSION/.gdbinit singular-$VERSION/*/.gdbinit singular-$VERSION/*/*/.gdbinit singular-$VERSION/*/*/*/.gdbinit
command rm singular-$VERSION/IntegerProgramming/README
command rm -rf singular-$VERSION/modules
command rm -rf singular-$VERSION/factory/old
cd singular-$VERSION
./autogen.sh
cd ..
command rm -rf singular-$VERSION/autom4te.cache
command rm -rf singular-$VERSION/*/autom4te.cache
command rm -rf singular-$VERSION/*/*/autom4te.cache
command rm -rf singular-$VERSION/omalloc/Misc
command rm -rf singular-$VERSION/Singular/LIB/surfex
command rm -rf singular-$VERSION/Singular/cnf2ideal.py
command rm -rf singular-$VERSION/kernel/old singular-$VERSION/kernel/sample
command rm -rf singular-$VERSION/dox/Doxyfile.html.all singular-$VERSION/dox/Doxyfile.in1 singular-$VERSION/dox/Doxyfile.latex.short singular-$VERSION/dox/readme
command rm singular-$VERSION/factory/ConwayList.txt
command rm -rf singular-$VERSION/logo  singular-$VERSION/standalone.test  singular-$VERSION/templates  singular-$VERSION/tests
cp redhat/singular.spec singular-$VERSION/redhat/singular.spec
tar cf singular-$VERSION.tar singular-$VERSION
gzip -9 -f singular-$VERSION.tar
command rm -rf singular-$VERSION
