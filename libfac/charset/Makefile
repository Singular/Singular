# static char rcsid[] = "@(#) $Id: Makefile,v 1.1.1.1 1997-05-02 17:00:44 Singular Exp $";
################################################
# testcs:   CharSet(F)
# testgcs:  GeneralCharSet(F,CharSetN)
# testgbs:  GeneralCharSet(F,BasicSet)
#################################################
factoryversion = 1.2b
factorypath = ../../factory/$(factoryversion)
RM = /bin/rm -f
MV = /bin/mv
AR = /usr/5bin/ar cr
RANLIB = ranlib
LIBPREF = ../libfac
OPTMARK =
LIBFAC = $(LIBPREF)$(OPTMARK).a

ALLFLAGS = -fno-implicit-templates#-DUSE_GCC263
# define the debugging flags here
#
DEFINES = $(ALLFLAGS) -DREORDERDEBUG#-DIRRCHARSERIESDEBUG# -DMCHARSETNDEBUG -DBASICSETDEBUG -DIRRASDEBUG
WARN = -Wall
DEBFLAGS= -g $(DEFINES) $(ALLFLAGS)
OPTFLAGS = -O3 -pipe $(DEFINES) $(ALLFLAGS)
#OPTFLAGS = -O3 -m486 
IFLAGS =  -I/usr/local/include -I$(factorypath) -I. -I../factor
CXX = gcc
CXXFLAGS = $(WARN)  $(IFLAGS)

CC = gcc
CFLAGS = $(WARN)  $(IFLAGS)

LD = g++
LDFLAGS=
LIBPATH = -L/usr/local/lib -L$(factorypath) -L..
AR = ar cr
RANLIB = ranlib

YACC = bison++

.SUFFIXES:      .cc .h .o

../DEBOBJ/%.o: %.cc ; $(CXX) -c $< $(CXXFLAGS) $(DEBFLAGS) -o $@

../OPTOBJ/%.o: %.cc ; $(CXX) -c $< $(CXXFLAGS) $(OPTFLAGS) -o $@

../DEBOBJ/%.o: %.c ; $(CC) -c $< $(CFLAGS) $(DEBFLAGS) -o $@

../OPTOBJ/%.o: %.c ; $(CC) -c $< $(CFLAGS) $(OPTFLAGS) -o $@


CHARSETSRC1 =	csutil.cc charset.cc  reorder.cc

CHARSETSRC2 =	t.C

CHARSETINCL =	csutil.h charset.h  reorder.h

CHARSETOPTOBJ = ../OPTOBJ/csutil.o ../OPTOBJ/charset.o \
		../OPTOBJ/reorder.o

CHARSETDEBOBJ =	../DEBOBJ/csutil.o ../DEBOBJ/charset.o \
		../DEBOBJ/reorder.o

LIBS = -lfac -lcf -lmem -lgmp -lm #-lPW
OPTLIBS = -lfac -lcf-optimize -lmem-optimize -lgmp -lm
DEBLIBS = -lfac-g -lcf-debug -lmem-debug -lgmp -lm

OPTTESTPROG = opttestcs 

DEBTESTPROG = debtestcs 

NORMALTESTPROG = normaltestcs 

TESTPROG = testcs 

debug:          $(DEBTESTPROG)

optimize:       $(OPTTESTPROG)

normal:		$(NORMALTESTPROG)

all:    debug

../OPTOBJ/optcsdepend:
	../../bin/makedep -P OPTOBJ $(DEFINES) $(IFLAGS) $(CHARSETSRC1)  > $@

../DEBOBJ/debcsdepend:
	../../bin/makedep -P DEBOBJ $(DEFINES) $(IFLAGS) $(CHARSETSRC1) > $@

newdepend: $(CHARSETSRC1) $(CHARSETINCL) 
	../../bin/makedep -P ../OPTOBJ $(DEFINES) $(IFLAGS) $(CHARSETSRC1) \
				> ../OPTOBJ/optcsdepend
	../../bin/makedep -P ../DEBOBJ $(DEFINES) $(IFLAGS) $(CHARSETSRC1) \
				> ../DEBOBJ/debcsdepend

OPTOBJ/testcs.o:	$(CHARSETOPTOBJ) testcs.cc $(CLAPHEADER)
	$(CXX) -c testcs.cc $(WARN) $(OPTFLAGS) $(IFLAGS) -o $@

DEBOBJ/testcs.o:	$(CHARSETDEBOBJ) testcs.cc $(CLAPHEADER)
	$(CXX) -c testcs.cc  $(DEBFLAGS) $(IFLAGS) -o $@

opttestcs:	OPTOBJ/testcs.o 
	$(LD) -o testcs $(LIBPATH) OPTOBJ/testcs.o $(CHARSETOPTOBJ) $(OPTLIBS)

debtestcs:       DEBOBJ/testcs.o 
	$(LD) -o testcs $(LIBPATH) DEBOBJ/testcs.o $(CHARSETDEBOBJ) $(DEBLIBS)

normaltestcs:	OPTOBJ/testcs.o
	$(LD) -o testcs $(LIBPATH) OPTOBJ/testcs.o $(CHARSETOPTOBJ) $(LIBS)

testscs:	debtestcs

OPTOBJ/testmcs.o:	$(CHARSETOPTOBJ) testmcs.cc $(CLAPHEADER)
	$(CXX) -c testmcs.cc $(WARN) $(OPTFLAGS) $(IFLAGS) -o $@

DEBOBJ/testmcs.o:	$(CHARSETDEBOBJ) testmcs.cc $(CLAPHEADER)
	$(CXX) -c testmcs.cc  $(DEBFLAGS) $(IFLAGS) -o $@

opttestmcs:	OPTOBJ/testmcs.o 
	$(LD) -o testmcs $(LIBPATH) OPTOBJ/testmcs.o $(CHARSETOPTOBJ) $(OPTLIBS)

debtestmcs:       DEBOBJ/testmcs.o 
	$(LD) -o testmcs $(LIBPATH) DEBOBJ/testmcs.o $(CHARSETDEBOBJ) $(DEBLIBS)

normaltestmcs:	OPTOBJ/testmcs.o
	$(LD) -o testmcs $(LIBPATH) OPTOBJ/testmcs.o $(CHARSETOPTOBJ) $(LIBS)

testmcs:	debtestmcs
	
OPTOBJ/testgbs.o:	$(CHARSETOPTOBJ) testgbs.cc $(CLAPHEADER)
	$(CXX) -c testgbs.cc $(WARN) $(OPTFLAGS) $(IFLAGS) -o $@

DEBOBJ/testgbs.o:	$(CHARSETDEBOBJ) testgbs.cc $(CLAPHEADER)
	$(CXX) -c testgbs.cc  $(DEBFLAGS) $(IFLAGS) -o $@

opttestgbs:	OPTOBJ/testgbs.o 
	$(LD) -o testgbs $(LIBPATH) OPTOBJ/testgbs.o $(CHARSETOPTOBJ) $(OPTLIBS)

debtestgbs:       DEBOBJ/testgbs.o 
	$(LD) -o testgbs $(LIBPATH) DEBOBJ/testgbs.o $(CHARSETDEBOBJ) $(DEBLIBS)

normaltestgbs:	OPTOBJ/testgbs.o
	$(LD) -o testgbs $(LIBPATH) OPTOBJ/testgbs.o $(CHARSETOPTOBJ) $(LIBS)

OPTOBJ/t.o:	$(CHARSETOPTOBJ) t.C $(CLAPHEADER)
	$(CXX) -c t.C  $(OPTFLAGS) $(IFLAGS) -o $@

DEBOBJ/t.o:	$(CHARSETDEBOBJ) t.C $(CLAPHEADER)
	$(CXX) -c t.C  $(DEBFLAGS) $(IFLAGS) -o $@

t:		OPTOBJ/t.o
	$(LD) -o t $(LIBPATH) OPTOBJ/t.o $(CHARSETOPTOBJ) $(OPTLIBS)

debt:		DEBOBJ/t.o
	$(LD) -o t $(LIBPATH) DEBOBJ/t.o $(CHARSETDEBOBJ) $(DEBLIBS)


libfac:         $(CHARSETOPTOBJ)
		$(AR) $(LIBFAC) $(CHARSETOPTOBJ)
		$(RANLIB) $(LIBFAC)

clean:
	$(RM)   $(TESTPROG) $(TESTPROG2)

cleanall:
	$(RM)   ../OPTOBJ/optcsdepend ../DEBOBJ/debcsdepend  \
		OPTOBJ/optcsdepend DEBOBJ/debcsdepend  \
		DEBOBJ/*.o OPTOBJ/*.o \
		$(TESTPROG) 

archive:	clean
	rm -f ../cxx.cpz
	gfind .	-path './OPT*' -prune -o \
		-path './DEB*' -prune -o \
		-print | cpio -o -a -c | gzip > ../cxx.cpz

disk:		archive
	mcopy ../cxx.cpz a:\cxx.cpz
	mcopy ../cxx.cpz a:\cxx1.cpz

unpack:
	gzcat ../cxx.cpz | cpio -i -m -c

dirs:
	mkdir OPTOBJ
	mkdir DEBOBJ

tests:	opttestcs
	(cd tests ; charsettest )

#include ../OPTOBJ/optcsdepend

#include ../DEBOBJ/debcsdepend

