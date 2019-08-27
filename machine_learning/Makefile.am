ACLOCAL_AMFLAGS = -I ../m4

AM_CPPFLAGS = -I/usr/include/python2.7 -DDATA_PATH=\"$(datadir)\"

libmlearn_LTLIBRARIES = libmlearn.la
libmlearndir = $(libdir)

SOURCES = mlpredict.c
LIBMLEARNHEADERS = mlpredict.h

libmlearn_la_SOURCES 	= $(SOURCES)
libmlearn_la_LDFLAGS 	= $(SINGULAR_LDFLAGS) -lpython2.7

libmlearn_includedir=${includedir}/singular/machine_learning
nobase_libmlearn_include_HEADERS= $(LIBMLEARNHEADERS)


###########################################################################

TESTS = test_bufsize test_init_ml

check_PROGRAMS = test_bufsize test_init_ml

test_bufsize_LDADD=libmlearn.la
test_bufsize_LDFLAGS= -lpython2.7

test_bufsize_SOURCES=tests/test_bufsize.c
test_bufsize_CFLAGS= -Wall -Wextra

test_init_ml_LDADD=libmlearn.la
test_init_ml_LDFLAGS= -lpython2.7

test_init_ml_SOURCES=tests/test_init_ml.c
test_init_ml_CFLAGS= -Wall -Wextra

#mlmoduledir = ${datadir}/singular/mlearn
nobase_dist_data_DATA = \
	ml_python/__init__.py \
	ml_python/predictor_runner.py \
	ml_python/common/__init__.py \
	ml_python/common/constants.py \
	ml_python/common/keyword_vector.py \
	ml_python/common/lookuptable.py \
	ml_python/model/__init__.py \
	ml_python/model/predictor.py \
	ml_singular/extract.lib
