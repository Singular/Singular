#!/bin/sh
#
# FILE:    gccversion
# USAGE:   gccversion
# PURPOSE: creates si_gcc_v.h with SI_GCC2 or SI_GCC_3 defined
# AUTHOR:  hannes
# CREATED: June 2008
#
#####################################################

if test "x$CC" = x
then
  CC=cc
fi
cat >tmp.c <<EOF
#include <stdio.h>
int main() {
FILE *f=fopen("si_gcc_v.h","w");
#ifdef __GNUC__
char *p=__VERSION__;
if (p[0]=='2') fprintf(f,"#define SI_GCC2\n");
else if ((p[0]=='3')||(p[0]=='4')) fprintf(f,"#define SI_GCC3\n");
#endif
fclose(f);
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
touch si_gcc_v.h
