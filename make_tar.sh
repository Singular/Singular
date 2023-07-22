#!/usr/bin/env bash
# Hannes helper to build tar files for the ftp server
# for the patch-versions (version between official releases x.x.x)
# for official versions: use "make dist"/"make distcheck"

set -e

TARVERSION=4.3.2p4
VERSION=4.3.2
BUILD_DIR=/tmp/wawa
export VERSION TARVERSION BUILD_DIR

# sanity check
if test -e $BUILD_DIR/singularconfig.h
then

rm -rf singular-$VERSION
git archive --prefix=singular-$VERSION/ HEAD |tar xf -

command rm singular-$VERSION/.gdbinit singular-$VERSION/*/.gdbinit singular-$VERSION/*/*/.gdbinit singular-$VERSION/*/*/*/.gdbinit
command rm singular-$VERSION/IntegerProgramming/README
cp doc/doc.tbz2 singular-$VERSION/doc/.
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
cp $BUILD_DIR/redhat/singular.spec singular-$VERSION/redhat/singular.spec
cp $BUILD_DIR/Singular/all.lib  singular-$VERSION/Singular/LIB/.
tar cf singular-$TARVERSION.tar singular-$VERSION
gzip -9 -f singular-$TARVERSION.tar
command rm -rf singular-$VERSION

else
  echo BUILD_DIR is not set correctly, use
  echo make dist
  echo in a configured directory
  exit 1
fi
