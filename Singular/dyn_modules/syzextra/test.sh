#!/bin/sh

#"$SINGULAR_EXECUTABLE" -teq "$srcdir/test_clear_enum.tst" || exit 1
#"$SINGULAR_EXECUTABLE" -teq "$srcdir/ederc.tst" || exit 1
#"$SINGULAR_EXECUTABLE" -teq "$srcdir/syzextra.tst" || exit 1
"$SINGULAR_EXECUTABLE" -tec 'LIB "schreyer.lib"; listvar(Top); proc T(){ Schreyer::testSimple(1); /* Schreyer::testAGR(0); Schreyer::testAGRhard(0); */ } T(); $' || exit 1
