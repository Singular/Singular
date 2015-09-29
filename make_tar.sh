#!/bin/sh
VERSION=4.0.2p1
export VERSION

git archive --prefix=singular-$VERSION/ HEAD |tar xf -
mkdir singular-$VERSION/doc
cp doc/*.man singular-$VERSION/doc/.
tar cf singular-$VERSION.tar singular-$VERSION
gzip -9 -f singular-$VERSION.tar
command rm -rf singular-$VERSION
