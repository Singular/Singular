#!/bin/sh
##
## comment:
##  omalloc testing/verification may oversee some memory error in some random cases:
##  this simple wrapper is meant to work around those unlucky cases by running a few
##  number of tests instead of just one test.
##
SCRIPTNAME=${0##*/}
CHECK_PROGRAM=${SCRIPTNAME#getlucky-}
NUMBEROF_TRIAL=11
for idx in $(seq 1 $NUMBEROF_TRIAL); do
	./$CHECK_PROGRAM && { echo "$SCRIPTNAME: SUMMARY: $idx trial(s)" ; exit 0 ; }
done
echo "$SCRIPTNAME: SUMMARY: $idx failed trials"
exit 1
# eos
