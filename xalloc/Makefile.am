ACLOCAL_AMFLAGS = -I ../m4

noinst_LTLIBRARIES=libomalloc.la

# lib_LTLIBRARIES=libomalloc.la
# lib_LIBRARIES=libomalloc.a

#if WANT_DEBUG
#lib_LTLIBRARIES+=libomalloc_g.la
#endif


## libomallocdir = $(libdir)/

libomalloc_includedir=$(includedir)/omalloc

libomalloc_include_HEADERS = omalloc.h

SOURCES = dummy.c

AM_CPPFLAGS = -I${srcdir} -DHAVE_CONFIG_H 

libomalloc_la_SOURCES  =$(SOURCES)
#libomalloc_g_la_SOURCES=$(SOURCES)

libomalloc_la_LDFLAGS  = -release ${PACKAGE_VERSION}

#libomalloc_la_CFLAGS		= -O3 ${PIPE}
#libomalloc_g_la_CFLAGS	= -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE}

#libomalloc_la_CXXFLAGS	= -O3 ${PIPE}
#libomalloc_g_la_CXXFLAGS= -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE}

# libomalloc_la_CPPFLAGS  += ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# libomalloc_la_CPPFLAGS = ${AM_CPPFLAGS}

