ACLOCAL_AMFLAGS = -I ../m4

AM_CPPFLAGS = -I/usr/include/python2.7

libmlearn_LTLIBRARIES = libmlearn.la
libmlearndir = $(libdir)

SOURCES = mlpredict.c
LIBMLEARNHEADERS = mlpredict.h

libmlearn_la_SOURCES 	= $(SOURCES)
libmlearn_la_LDFLAGS 	= $(SINGULAR_LDFLAGS) -lpython2.7

libmlearn_includedir=${includedir}/singular/machine_learning
nobase_libmlearn_include_HEADERS= $(LIBMLEARNHEADERS)


###########################################################################

check_PROGRAMS = testml

testml_LDADD=libmlearn.la
testml_LDFLAGS= -lpython2.7

testml_SOURCES=testml.c
testml_CFLAGS= -Wall -Wextra
