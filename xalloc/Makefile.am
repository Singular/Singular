ACLOCAL_AMFLAGS = -I ../m4

if WANT_DEBUG
  LIB_G=libomalloc_g.la
else
  LIB_G=
endif

lib_LTLIBRARIES=libomalloc.la ${LIB_G}
## libomallocdir = $(libdir)/

libomalloc_includedir=$(includedir)/omalloc

libomalloc_include_HEADERS = omalloc.h

SOURCES = dummy.c

AM_CPPFLAGS = -I${srcdir}

libomalloc_la_SOURCES  =$(SOURCES)
libomalloc_g_la_SOURCES=$(SOURCES)

libomalloc_la_CFLAGS		= -O3 ${PIPE}
libomalloc_g_la_CFLAGS	= -g -Wextra -Winline -Wall -pedantic -Wno-long-long ${PIPE}

libomalloc_la_CXXFLAGS	= -O3 ${PIPE}
libomalloc_g_la_CXXFLAGS= -g -Wextra -Winline -Wall -pedantic -Wno-long-long ${PIPE}

libomalloc_la_CPPFLAGS  = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
libomalloc_g_la_CPPFLAGS= ${AM_CPPFLAGS} -DHAVE_CONFIG_H 

