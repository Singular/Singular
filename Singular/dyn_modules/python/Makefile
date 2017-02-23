CC	= gcc
CXX	= g++
PYTHON_VERSION=2.7
SINGULAR_BASE=../../..
CFLAGS	= -DSING_NDEBUG -DSING_OMNDEBUG -I. -I${SINGULAR_BASE} -I${SINGULAR_BASE}/kernel -I${SINGULAR_BASE}/Singular  -I/usr/include/python${PYTHON_VERSION} -I/tmp2/wawa -I${SINGULAR_BASE}/libpolys -I/tmp2/wawa/libpolys -I${SINGULAR_BASE}/factory/include
LDFLAGS= -lboost_python -lpython${PYTHON_VERSION}
#LD	=
MKINSTALLDIRS=mkdir -p
instdir          = /home/michael/sing/ix86-Linux
INSTALL		= /home/michael/sing/Singular/install-sh -c
INSTALL_PROGRAM	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
SRCS	= python.cc poly_wrap.cc vector_wrap.cc CF_wrap.cc\
             ņumber_wrap.cc 
OBJS	= python.o poly_wrap.o vector_wrap.o CF_wrap.o\
             number_wrap.o 

all:	python_module.so

%.o: %.cc Makefile
	${CXX} ${CFLAGS} -c -fPIC -DPIC $< -o $*.o

python_module.so: ${OBJS}
	${CXX} ${CFLAGS} ${LDFLAGS} -shared -Wl,-soname -Wl,python_module.so.1 \
		-o python_module.so.1.2.0 ${OBJS}
	rm -f python_module.so
	ln -s python_module.so.1.2.0 python_module.so

clean:
	rm -f *.o *.lo *.so* *.sl *.la *~ core

distclean: clean
	rm -f Makefile *.pl

install:
	${MKINSTALLDIRS} ${instdir}
	${MKINSTALLDIRS} ${instdir}/MOD
	${INSTALL_PROGRAM} python_module.so ${instdir}/MOD/python_module.so


