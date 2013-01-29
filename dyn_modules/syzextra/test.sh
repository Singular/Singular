#!/bin/sh

# env
"$SINGULAR_EXECUTABLE" -teq "$srcdir/test_clear_enum.tst"

"$SINGULAR_EXECUTABLE" -teq "$srcdir/syzextra.tst" && "$SINGULAR_EXECUTABLE" -teq "$srcdir/ederc.tst"

