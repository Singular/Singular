# Generated automatically from Makefile.in by configure.-
#################################################################
###
### Makefile for Singular
###
#################################################################

SHELL		= /bin/sh

##
## version
##
SINGULAR_VERSION	= spielwiese
##
## various paths
##
srcdir		= .
includedir      = ../libpolys

##
## various programs
##

CC		= gcc
LD              = ld
CXX		= g++
PERL		= perl
INSTALL		= /usr/bin/install -c
INSTALL_PROGRAM	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
MKINSTALLDIRS   = ./mkinstalldirs
LN_S		= ln -s

##
## compiler and linker options
##
LIBPOLYS_CFLAGS = -I.. -I../libpolys -I/opt/local/include
LIBPOLYS_LIBS   = -L../factory -L../omalloc -L../libpolys/misc -L../libpolys/reporter -L../libpolys/resources -L../libpolys/coeffs -L../libpolys/polys -lpolys -lcoeffs -lresources -lreporter -lmisc -lfactory -lomalloc -L/opt/local/lib -lntl -lgmp -ldl
LIBPOLYS_LIBSG  = -L../factory -L../omalloc -L../libpolys/misc -L../libpolys/reporter -L../libpolys/resources -L../libpolys/coeffs -L../libpolys/polys -lpolys_g -lcoeffs_g -lresources_g -lreporter_g -lmisc_g -lfactory -lomalloc_g -L/opt/local/lib  -lntl -lgmp -ldl

PIPE		= -pipe
CFLAGS		= -O3 -fomit-frame-pointer ${PIPE} ${LIBPOLYS_CFLAGS}
CXXFLAGS	= -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${LIBPOLYS_CFLAGS}
CXXTEMPLFLAGS	=  --no-exceptions ## -fno-implicit-templates
CPPFLAGS	= -I${srcdir} -I..  -I${includedir}   ${LIBPOLYS_CFLAGS}
DEFS		= -DNDEBUG -DOM_NDEBUG
LDFLAGS		= -L${srcdir}/lib 
LD_DYN_FLAGS	= -ldl -rdynamic
SFLAGS		= -fpic -DPIC
SLDFLAGS	= -shared

LD_LIBC	        = -lc
STATIC_LDFLAGS	= -static
LIBS		= -lm -lsingfac -lsingcf -lntl -lgmp -lreadline -lncurses -lm  -lnsl  -lomalloc_ndebug 
MP_LIBS		= -lMPT -lMP -lnsl 

## End configuration dependend stuff
#################################################################

###
### file sets
###

# normal C++ source files
CXXSOURCES=\
    polys.cc febase.cc feread.cc \
    hdegree.cc hilb.cc hutil.cc \
    gr_kstd2.cc \
    ideals.cc \
    khstd.cc kstdfac.cc \
    kstd1.cc kstd2.cc kutil.cc \
    misc.cc \
    fast_maps.cc \
    fglmzero.cc fglmvec.cc fglmgauss.cc fglmhom.cc fglmcomb.cc \
    kspoly.cc kpolys.cc \
    semic.cc \
    syz.cc syz0.cc syz1.cc syz2.cc syz3.cc\
    timer.cc \
    GMPrat.cc multicnt.cc npolygon.cc semic.cc spectrum.cc splist.cc \
    walkProc.cc walkMain.cc walkSupport.cc\
    eigenval.cc units.cc \
    fast_mult.cc digitech.cc \
    tgb.cc tgbgauss.cc ringgb.cc f5data.cc f5lists.cc f5gb.cc \
    f5c.cc ratgring.cc shiftgb.cc gfan.cc \
    linearAlgebra.cc nc.cc preimage.cc
    
CXXSOURCES2	= SingularBuilder.cpp

#    numbers.cc polys.cc p_polys.cc polys0.cc polys1.cc polys-impl.cc \

# normal C source files
CSOURCES=fegetopt.c mmstd.c

# special C++ source files (need extra compiling and/or linking), for which
# dependencies should be generated
ESOURCES=mmalloc.cc

SOURCES=${CSOURCES} ${CXXSOURCES} \
	kInline.h

HEADERS= polys.h hutil.h idrec.h stairc.h ideals.h \
	structs.h \
	syz.h \
	fast_maps.h \
	febase.h \
	walkProc.h walkMain.h walkSupport.h\
	khstd.h sparsmat.h \
	fglm.h kstd1.h \
	fglmgauss.h fglmvec.h \
	kstdfac.h kmatrix.h\
	kutil.h \
        GMPrat.h multicnt.h npolygon.h semic.h spectrum.h splist.h multicnt.h \
	eigenval.h units.h \
	fegetopt.h \
	ratgring.h shiftgb.h nc.h \
        mmalloc.h \
	semic.h \
	gfan.h preimage.h

DISTFILES=${SOURCES} ${HEADERS} ${ESOURCES} \
        Makefile.in \
	mod2.h.in install-sh mkinstalldirs

OBJS := $(CXXSOURCES:.cc=.o) $(CSOURCES:.c=.o) #$(CXXSOURCES2:.cpp=.o)
OBJS2 := $(ESOURCES:.cc=.o)
OBJSG2 := $(ESOURCES:.cc=.og)
OBJSP2 := $(ESOURCES:.cc=.op)

##
## Build Targets
##
.cc.o:	
	${CXX} ${CXXFLAGS} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFS} -c $<
.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} ${DEFS} -c $<

OBJG= $(CXXSOURCES:.cc=.og) $(CSOURCES:.c=.og)

OBJG := $(OBJG) $(STATIC_SOURCES:.cc=.og) 
OBJP := $(OBJP) $(STATIC_SOURCES:.cc=.op) 

all:	libkernel.a libkernel_g.a mmalloc.o ${OBJS2}

libkernel.a: ${OBJS}
	-rm -f libkernel.a
	ar cr $@ $^

##
## install targets
##
install-nolns: install

install-libsingular: install
	-${MKINSTALLDIRS} ${includedir}
	-${MKINSTALLDIRS} ${includedir}/singular
	-for file in *.h kInline.h; do sed -e "s:<kernel/:<singular/:"< $$file | sed -e "s:<Singular/:<singular/:"|sed -e "s:<omalloc/:<:"|sed -e "s:<factory/:<:" > ${includedir}/singular/$$file; done

install: all installbin

install_all: install libkernel_p.a
	- ranlib libkernel_p.a

installbin: libkernel.a libkernel_g.a
	${MKINSTALLDIRS} ${bindir}
	- ranlib libkernel.a
	- ranlib libkernel_g.a

install-bindist: libkernel.a
	${MKINSTALLDIRS} ${bindir}

##
## clean targest
##
mostlyclean: 
	-rm -f *.o *.og core *.op

clean: mostlyclean
	-rm -f *.bak *.d *.dd depend *.a *.so* test testg test-g

distclean: clean
	-rm -f *~ .\#* 
	-rm -f mod2.h Makefile TAGS* tags config.status config.cache config.log

srcclean:
	@echo "This command is intended for maintainers to use;"
	@echo "Rebuilding the deleted files requires flex"
	@echo "bison, perl"

maintainer-clean: distclean srcclean
	@echo "This command is intended for maintainers to use;"
	@echo "Rebuilding the deleted files requires flex"
	@echo "bison, perl and autoconf"

##
## miscellanous targets
##

TAGS:   ${SOURCES} ${ESOURCES} ${HEADERS} 
	etags ${SOURCES} ${ESOURCES} ${HEADERS}

tags:   
	ctags *.c *.h *.cc *.inc

##
## Below here is stuff for developpers
#################################################################


##
## .og files for having -O and -g object versions available at the same time
##

OBJG1 := $(CXXSOURCES:.cc=.og) 
OBJG2 := $(CSOURCES:.c=.og) 

##
## compiler and linker options for debug version
##

CCG		= ${CC}
CXXG		= ${CXX} -Wall
CXXM		= gcc -MM -MG -DGENERATE_DEPEND
CCM		= gcc -MM -MG -DGENERATE_DEPEND

CFLAGSG		= -g ${PIPE} ${LIBPOLYS_CFLAGS}
CXXFLAGSG	= -g ${PIPE} ${LIBPOLYS_CFLAGS}
DEFSG		= 

##
## Debug Targets
##

%.og: %.cc
	$(CXXG) ${CXXFLAGSG} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSG} -c $< -o $@

$(OBJG2) : %.og: %.c
	$(CCG)  ${CFLAGSG} ${CPPFLAGS} ${DEFSG} -c $< -o $@

libkernel_g.a: ${OBJG} ${OBJSG2}
	-rm -f libkernel_g.a
	ar cr $@ ${OBJG}



##
## .op files for profiling
##

OBJP1 := $(CXXSOURCES:.cc=.op) 
OBJP2 := $(CSOURCES:.c=.op) 

OBJP=$(OBJP1) $(OBJP2) 

OBJP := $(OBJP) $(STATIC_SOURCES:.cc=.op) 

##
## compiler and linker options for profiling version
##

CCP		= ${CC}
CXXP		= ${CXX}

CFLAGSP		= -g -pg -O ${PIPE} ${LIBPOLYS_CFLAGS}
CXXFLAGSP	= -g -pg -O ${PIPE} ${LIBPOLYS_CFLAGS}
DEFSP		= 




##
## Profiling Targets
##

%.op: %.cc
	$(CXXP) ${CXXFLAGSP} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSP} -c $< -o $@

$(OBJP2) : %.op: %.c
	$(CCP)  ${CFLAGSP} ${CPPFLAGS} ${DEFSP} -c $< -o $@

libkernel_p.a: ${OBJP} ${OBJSP2}
	-rm -f libkernel_p.a
	ar cr $@ ${OBJP}

##
## dependencies
##

%.dd: %.cc mod2.h 
	echo $(@:.dd=.og) $(@:.dd=.op) " " \\ > $@
	$(CXXM) -DGENTABLE ${CPPFLAGS} ${DEFSG} $< >> $@

%.d: %.c mod2.h 
	echo $(@:.d=.og) $(@:.d=.op) " " \\ > $@
	$(CCM) ${CPPFLAGS} ${DEFSG} $< >> $@

depend:   $(CXXSOURCES:.cc=.dd) $(ESOURCES:.cc=.dd) $(CSOURCES:.c=.d) mod2.h
	cat *.d *.dd >depend

ifeq (depend,$(wildcard depend))
include depend
endif


testg: test.og libkernel_g.a
	$(CXXG) ${CXXFLAGSG} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSG} $< -o $@ -L. -lkernel_g ${LIBPOLYS_LIBSG}

test: test.o libkernel.a
	$(CXX) ${CXXFLAGS} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFS} $< -o $@ -L. -lkernel ${LIBPOLYS_LIBS}

# My headers test
check: all test testg
	SINGULAR_ROOT_DIR=`pwd/../` SINGULARPATH="$SINGULAR_ROOT_DIR/libpolys/polys/.libs" ./test
	SINGULAR_ROOT_DIR=`pwd/../` SINGULARPATH="$SINGULAR_ROOT_DIR/libpolys/polys/.libs" ./testg
