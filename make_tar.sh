#!/bin/sh
VERSION=4.0.2p1
export VERSION

git archive --prefix=singular-$VERSION/ HEAD |tar xf -
mkdir singular-$VERSION/doc
cp doc/*.man singular-$VERSION/doc/.
command rm singular-$VERSION/.gdbinit singular-$VERSION/Singular/.gdbinit
command rm -rf singular-$VERSION/Singular/dyn_modules/bigintm/
command rm -rf singular-$VERSION/modules
tar cf singular-$VERSION.tar singular-$VERSION
gzip -9 -f singular-$VERSION.tar
command rm -rf singular-$VERSION
