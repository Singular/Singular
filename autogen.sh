#! /bin/sh

cd `dirname "$0"`

# -d --warnings=all
autoreconf  -v -f -i
find . -name configure -exec sed -i -e 's/\"-g -O2\"/\"-g\"/g' {} \; -print
touch */configure */configure.ac */aclocal.m4 */configure */Makefile.am */Makefile.in */_config.h.in configure aclocal.m4 Makefile.am Makefile.in _config.h.in

cd -

