#!/bin/sh
#
# FILE:    gccversion
# USAGE:   gccversion
# PURPOSE: prints to stdout the gcc version type
# AUTHOR:  hannes
# CREATED: May 2003
#
#####################################################

if test "x$CC" = x
then
  CC=cc
fi
cat >tmp.c <<EOF
int main() {
#ifdef __GNUC__
char *p=__VERSION__;
if (p[0]=='2') printf("gcc2\n");
else if ((p[0]=='3')||(p[0]=='4')) printf("gcc3\n");
else printf("gcc_unknown\n");
#else
printf("unknown\n");
#endif
exit(0); }
EOF
$CC tmp.c
if test -f ./a.out; then
  ./a.out
  /bin/rm -f a.out tmp.c
elif test -f a.exe; then
  ./a.exe
  /bin/rm -f tmp.c a.exe
else
  echo run_unknown
fi
