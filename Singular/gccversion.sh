#!/bin/sh
#
# FILE:    gccversion
# USAGE:   gccversion
# PURPOSE: prints to stdout the gcc version type
# AUTHOR:  hannes
# CREATED: May 2003
#
###########################################################################

egrep="egrep"
uname_a=`uname -a`
devnull='/dev/null'
file=file
binary='/bin/ls'
ldd='ldd'

if test x$CC = x
then
CC=cc
fi
cat >tmp.c <<EOF
int main() {
#ifdef __GNUC__
char *p=__VERSION__;
if (p[0]=='2') printf("gcc2\n");
else if (p[0]=='3') printf("gcc3\n");
else printf("gcc_unknown\n");
#else
printf("unknown\n");
#endif
exit(0); }
EOF
$CC tmp.c
./a.out
/bin/rm -f a.out tmp.c
