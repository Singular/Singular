#!/bin/sh
VERSION=4.0.2p1
export VERSION

git archive --prefix=singular-$VERSION/ HEAD |tar xf -
mkdir singular-$VERSION/doc
cp doc/*.man singular-$VERSION/doc/.
command rm singular-$VERSION/.gdbinit singular-$VERSION/*/.gdbinit singular-$VERSION/*/*/.gdbinit singular-$VERSION/*/*/*/.gdbinit
command rm -rf singular-$VERSION/modules
command rm -rf singular-$VERSION/factory/old
command rm -rf singular-$VERSION/autom4te.cache
command rm -rf singular-$VERSION/*/autom4te.cache
command rm -rf singular-$VERSION/*/*/autom4te.cache
cd singular-$VERSION
./autogen.sh
cd ..
tar cf singular-$VERSION.tar singular-$VERSION
gzip -9 -f singular-$VERSION.tar
command rm -rf singular-$VERSION
