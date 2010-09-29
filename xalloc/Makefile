#################################################################
###
### Makefile for xalloc
###
#################################################################

SHELL		= /bin/sh

CC		= gcc
AR              = ar
RANLIB          = ranlib
INSTALL		= install -c
INSTALL_PROGRAM	= ${INSTALL} 
INSTALL_DATA	= ${INSTALL} -m 644

##
## compiler and linker options
##
PIPE		= -pipe
CFLAGS		= -O3 -w -fomit-frame-pointer ${PIPE}
CXXFLAGS	= -O3 -w -fomit-frame-pointer ${PIPE}
#CFLAGS		= -g ${PIPE}
#CXXFLAGS	= -g ${PIPE}

# normal C source files
CSOURCES=omFindExec.c

HEADERS=omalloc.h

OBJS := $(CSOURCES:.c=.o)

.cc.o:	
	${CXX} ${CXXFLAGS} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFS} -c $<
.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} ${DEFS} -c $<

all:	libomalloc_ndebug.a

libomalloc_ndebug.a: $(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm *.o *.a
