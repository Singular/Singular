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
LIBPOLYS_INCLUDES = -I.. -I../libpolys -I/opt/local/include

LIBPOLYS_CFLAGS   = ${LIBPOLYS_INCLUDES}

LIBPOLYS_LIBS     = -L../factory -L../omalloc -L../libpolys/misc -L../libpolys/reporter -L../libpolys/resources -L../libpolys/coeffs -L../libpolys/polys -lpolys -lcoeffs -lresources -lreporter -lmisc -lfactory -lomalloc -L/opt/local/lib -lntl -lgmp -ldl

LIBPOLYS_LIBSG    = -L../factory -L../omalloc -L../libpolys/misc -L../libpolys/reporter -L../libpolys/resources -L../libpolys/coeffs -L../libpolys/polys -lpolys_g -lcoeffs_g -lresources_g -lreporter_g -lmisc_g -lfactory -lomalloc_g  -L/opt/local/lib  -lntl -lgmp -ldl


PYTHON_INCLUDES = 
# `python2-config --includes`
PYTHON_CFLAGS = 
# `python2-config --cflags`
PYTHON_LIBS   = 
# `python2-config --libs`

PIPE		= -pipe
CFLAGS		= -O3 -Wall -fomit-frame-pointer ${PIPE} ${LIBPOLYS_CFLAGS} ${PYTHON_CFLAGS}
CXXFLAGS	= -O3 -Wall -fomit-frame-pointer --no-rtti ${PIPE} ${LIBPOLYS_CFLAGS} ${PYTHON_CFLAGS}
CXXTEMPLFLAGS	=  --no-exceptions ## -fno-implicit-templates
CPPFLAGS	= -I${srcdir} -I..  -I${includedir} ${LIBPOLYS_INCLUDES}  ${PYTHON_INCLUDES}
DEFSNL		= -DNDEBUG -DOM_NDEBUG
DEFS		= ${DEFSNL} -DLIBSINGULAR
LDFLAGS		= -L${srcdir}/lib 
LD_DYN_FLAGS	= -ldl -rdynamic
SFLAGS		= -fpic -DPIC
SLDFLAGS	= -shared

LD_LIBC	        = -lc
STATIC_LDFLAGS	= -static
LIBS		= -lm -lsingfac -lsingcf -lntl -lgmp -lreadline -lncurses -lm  -lnsl  -lomalloc_ndebug ${PYTHON_LIBS}
MP_LIBS		= -lMPT -lMP -lnsl 

## End configuration dependend stuff
#################################################################

###
### file sets
###

# special C++ source files (need extra compiling and/or linking), for which
# dependencies should be generated
ESOURCES=claptmpl.cc

# normal C++ source files
CXXSOURCES=\
   Minor.cc\
   MinorInterface.cc\
   MinorProcessor.cc\
   attrib.cc\
   bbcone.cc\
   bbfan.cc\
   bigintm.cc\
   blackbox.cc\
   calcSVD.cc\
   cntrlc.cc\
   denom_list.cc\
   eigenval_ip.cc\
   extra.cc\
   feOpt.cc\
   fehelp.cc\
   fevoices.cc \
   fglm.cc\
   gms.cc\
   grammar.cc\
   interpolation.cc\
   iparith.cc\
   ipassign.cc\
   ipconv.cc\
   ipid.cc\
   iplib.cc\
   ipprint.cc\
   ipshell.cc\
   janet.cc\
   libparse.cc\
   linearAlgebra_ip.cc\
   lists.cc\
   maps_ip.cc\
   minpoly.cc\
   misc_ip.cc\
   mpsr_Error.cc\
   mpsr_Get.cc\
   mpsr_GetMisc.cc\
   mpsr_GetPoly.cc\
   mpsr_Put.cc\
   mpsr_PutPoly.cc\
   mpsr_Timer.cc\
   mpsr_Tok.cc\
   mpsr_sl.cc\
   ndbm.cc\
   newstruct.cc\
   pcv.cc\
   pipeLink.cc\
   scanner.cc\
   sdb.cc\
   silink.cc\
   sing_dbm.cc\
   sing_win.cc\
   slInit_Dynamic.cc\
   slInit_Static.cc\
   ssiLink.cc\
   subexpr.cc\
   utils.cc\
   walk.cc\
   walk_ip.cc\
   wrapper.cc ${ESOURCES} 

#   tesths.cc

#   pyobject.cc  pyobject_setup.cc

CXXSOURCES2	= SingularBuilder.cpp

CSOURCES = fereadl.c mmstd.c 

SOURCES=${CSOURCES} ${CXXSOURCES} \
	kInline.h

HEADERS=\
   Cache.h \
   CacheImplementation.h \
   Minor.h \
   MinorInterface.h \
   MinorProcessor.h \
   attrib.h \
   bbcone.h \
   bigintm.h \
   blackbox.h \
   cntrlc.h \
   dbm_sl.h \
   distrib.h \
   eigenval_ip.h \
   feOpt.h \
   fglm.h \
   gms.h \
   grammar.h \
   interpolation.h \
   ipconv.h \
   ipid.h \
   ipprint.h \
   ipshell.h \
   janet.h \
   libparse.h \
   libsingular.h \
   lists.h \
   locals.h \
   maps_ip.h \
   minpoly.h \
   misc_ip.h \
   mmalloc.h \
   omSingularConfig.h \
   mpsr.h \
   mpsr_Get.h \
   mpsr_Put.h \
   mpsr_Timer.h \
   mpsr_Tok.h \
   mpsr_sl.h \
   ndbm.h \
   newstruct.h \
   pcv.h \
   pipeLink.h \
   pyobject_setup.h \
   run.h \
   sdb.h \
   silink.h \
   sing_dbm.h \
   sing_win.h \
   slInit.h \
   ssiLink.h \
   static.h \
   stype.h \
   subexpr.h \
   table.h \
   tok.h \
   utils.h \
   walk.h


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

OBJG= $(OBJS:.o=.og)

OBJG := $(OBJG) $(STATIC_SOURCES:.cc=.og) 
OBJP := $(OBJP) $(STATIC_SOURCES:.cc=.op) 

all: libSingular_g.a libSingular.a Singular Singularg 

tesths.og: %.og: %.cc
	$(CXXG) ${CXXFLAGSG} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSGNL} -c $< -o $@
	
tesths.o: %.o: %.cc
	$(CXX) ${CXXFLAGS} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSNL} -c $< -o $@

Singular: tesths.o libSingular.a
	$(CXX) ${CXXFLAGS} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFS} tesths.o -o $@ -L. -lSingular -L../kernel -lkernel -L../numeric -lnumeric ${LIBPOLYS_LIBS}

Singularg: tesths.og libSingular_g.a
	$(CXXG) ${CXXFLAGSG} ${CXXTEMPLFLAGSG} ${CPPFLAGSG} ${DEFSG} tesths.og -o $@ -L. -lSingular_g -L../kernel -lkernel_g -L../numeric -lnumeric_g ${LIBPOLYS_LIBSG}


##
## clean targest
##
mostlyclean: 
	-rm -f *.o *.og core *.op

clean: mostlyclean
	-rm -f *.bak *.d *.dd depend *.a *.so*
	-rm feOpt.inc iparith.inc plural_cmd.inc

distclean: clean
	-rm -f *~ .\#* 
	-rm -f Makefile TAGS* tags config.status config.cache config.log

srcclean:
	@echo "This command is intended for maintainers to use;"
	@echo "Rebuilding the deleted files requires flex"
	@echo "bison, perl"

maintainer-clean: distclean srcclean
	@echo "This command is intended for maintainers to use;"
	@echo "Rebuilding the deleted files requires flex"
	@echo "bison, perl and autoconf"

#dep. ( manually?!! :((( )
feOpt.h: feOpt.inc
cntrlc.cc: feOpt.inc
iparith.cc: iparith.inc
misc_ip.cc: feOpt.h

mpsr_Tok.inc: gentable2.cc
	${CXX} ${CXXFLAGS} ${CPPFLAGS} ${CXXTEMPLFLAGS} ${DEFS} gentable2.cc -o gentable2
	./gentable2
	/bin/rm -f gentable2 gentable2.exe


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

CCG		= ${CC} -g
CXXG		= ${CXX} -g
CXXM		= gcc -MM -MG -DGENERATE_DEPEND
CCM		= gcc -MM -MG -DGENERATE_DEPEND

DEFSGNL		= 
DEFSG		= ${DEFSGNL} -DLIBSINGULAR

CFLAGSG		= -Wall ${PIPE} ${LIBPOLYS_CFLAGS} ${PYTHON_CFLAGS}
CXXFLAGSG	= -Wall ${PIPE} ${LIBPOLYS_CFLAGS} ${PYTHON_CFLAGS}

##
## Debug Targets
##

%.og: %.cc
	$(CXXG) ${CXXFLAGSG} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSG} -c $< -o $@

$(OBJG2) : %.og: %.c
	$(CCG)  ${CFLAGSG} ${CPPFLAGS} ${DEFSG} -c $< -o $@


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

CCP		= ${CC} -g -pg 
CXXP		= ${CXX} -g -pg 

CFLAGSP		= -O ${PIPE} ${LIBPOLYS_CFLAGS}
CXXFLAGSP	= -O ${PIPE} ${LIBPOLYS_CFLAGS}
DEFSP		= 




##
## Profiling Targets
##

%.op: %.cc
	$(CXXP) ${CXXFLAGSP} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSP} -c $< -o $@

$(OBJP2) : %.op: %.c
	$(CCP)  ${CFLAGSP} ${CPPFLAGS} ${DEFSP} -c $< -o $@

##
## dependencies
##

%.dd: %.cc
	echo $(@:.dd=.og) $(@:.dd=.op) " " \\ > $@
	$(CXXM) -DGENTABLE ${CPPFLAGS} ${DEFSG} $< >> $@

%.d: %.c
	echo $(@:.d=.og) $(@:.d=.op) " " \\ > $@
	$(CCM) ${CPPFLAGS} ${DEFSG} $< >> $@

depend:   $(CXXSOURCES:.cc=.dd) $(ESOURCES:.cc=.dd) $(CSOURCES:.c=.d)
	cat *.d *.dd >depend

ifeq (depend,$(wildcard depend))
include depend
endif


# My headers test
# test: test.o

feOpt.inc: feOpt.cc
	$(CXX) $(CXXFLAGS) -DGENERATE_OPTION_INDEX ${CPPFLAGS} feOpt.cc -o feOpt
	./feOpt
	/bin/rm -f feOpt feOpt.exe

feOptES.inc: feOpt.cc
	$(CXX) $(CXXFLAGS) -DGENERATE_OPTION_INDEX ${CPPFLAGS} -DESINGULAR -I../kernel feOpt.cc -o feOptES
	./feOptES
	rm -f feOptES feOptES.exe

feOptTS.inc: feOpt.cc
	$(CXX) $(CXXFLAGS) -DGENERATE_OPTION_INDEX ${CPPFLAGS} -DTSINGULAR -I../kernel feOpt.cc -o feOptTS
	./feOptTS
	rm -f feOptTS feOptTS.exe

plural_cmd.inc: gentable.cc grammar.h tok.h table.h
	 ${CXX} ${CXXFLAGS} ${CPPFLAGS} ${CXXTEMPLFLAGS} ${DEFS} gentable.cc \
	     -o gentable1
	./gentable1
	/bin/rm -f gentable1 gentable1.exe

iparith.inc: gentable.cc grammar.h tok.h table.h
	 ${CXX} ${CXXFLAGS} ${CPPFLAGS} ${CXXTEMPLFLAGS} ${DEFS} gentable.cc \
	     -o gentable1
	./gentable1
	/bin/rm -f gentable1 gentable1.exe



libSingular.a: ${OBJS}
	-rm -f $@
	ar cr $@ $^


libSingular_g.a: ${OBJG}
	-rm -f $@
	ar cr $@ $^

test: test.o libSingular.a
	$(CXX) ${CXXFLAGS} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFS} $< -o $@  -L. -lSingular -L../kernel -lkernel -L../numeric -lnumeric ${LIBPOLYS_LIBS}

# My headers test
check: test testg
	./test
	./testg

testg: test.og libSingular_g.a
	$(CXXG) ${CXXFLAGSG} ${CXXTEMPLFLAGS} ${CPPFLAGS} ${DEFSG} $< -o $@  -L. -lSingular_g -L../kernel -lkernel_g -L../numeric -lnumeric_g ${LIBPOLYS_LIBSG}
