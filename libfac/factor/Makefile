# static char rcsid[] = "@(#) $Id: Makefile,v 1.2 1997-06-09 15:56:01 Singular Exp $";
#############################################################################
# testcs:   CharSet(F)
# testgcs:  GeneralCharSet(F,CharSetN)
# testgbs:  GeneralCharSet(F,BasicSet)
#################################################
factoryversion = 1.1
factorypath = ../factory/$(factoryversion)
RM = /bin/rm -f
MV = /bin/mv
AR = ar cr
RANLIB = ranlib
LIBPREF = libfac
OPTMARK =
LIBFAC = $(LIBPREF)$(OPTMARK).a
LIBFAC-G = $(LIBPREF)-g$(OPTMARK).a 
STRIP = strip
# define the debugging flags here
#
FACDEFINES = -DFACTORDEBUG -DSQRFREEDEBUG -DTRUEFACTORDEBUG -DHENSELDEBUG -DHENSELDEBUG2 -DNEWCODE
CHARDEFINES = #-DBASICSETDEBUG -DMCHARSETDEBUG 
DEFINES = $(FACDEFINES) $(CHARDEFINES)
WARN = -Wall
ALLFLAGS = -fno-implicit-templates#-fexternal-templates#-DUSE_GCC263
DEBFLAGS= -g $(DEFINES) $(ALLFLAGS)
DEBFLAGSTMPL= -g $(DEFINES) 
OPTFLAGS = -O3 $(ALLFLAGS) 
#OPTFLAGS = -O6 -m486 $(ALLFLAGS)
OPTFLAGSTMPL= -O3
#OPTFLAGSTMPL= -O6

IFLAGS =  -I/usr/local/include -I$(factorypath) -I. -I./factor
CXX = gcc 
OPTCXX = gcc
CXXFLAGS = $(WARN) $(IFLAGS) $(DEFINES)

CC = gcc
OPTCC = gcc -s
CFLAGS = $(WARN) $(IFLAGS) $(DEFINES)

LD = g++ $(ALLFLAGS)
OPTLD = g++ -s $(ALLFLAGS) 
LDFLAGS=
LIBPATH = -L/usr/local/lib -L$(factorypath) -L.

.SUFFIXES:      .cc .h .o

DEBOBJ/%.o: %.cc ; $(CXX) -c $< $(CXXFLAGS) $(DEBFLAGS) -o $@

OPTOBJ/%.o: %.cc ; $(OPTCXX) -c $< $(CXXFLAGS) $(OPTFLAGS) -o $@

DEBOBJ/%.o: charset/%.cc ; $(CXX) -c $< $(CXXFLAGS) $(DEBFLAGS) -o $@

OPTOBJ/%.o: charset/%.cc ; $(OPTCXX) -c $< $(CXXFLAGS) $(OPTFLAGS) -o $@

DEBOBJ/%.o: factor/%.cc ; $(CXX) -c $< $(CFLAGS) $(DEBFLAGS) -o $@

OPTOBJ/%.o: factor/%.cc ; $(OPTCXX) -c $< $(CFLAGS) $(OPTFLAGS) -o $@

DEBOBJ/%.o: %.C ; $(CXX) -c $< $(CXXFLAGS) $(DEBFLAGS) -o $@

OPTOBJ/%.o: %.C ; $(OPTCXX) -c $< $(CXXFLAGS) $(OPTFLAGS) -o $@

OPTTESTPROG = opttest

DEBTESTPROG = debtest

NORMALTESTPROG = normaltest

default:	$(DEBTESTPROG)

optimize:       $(OPTTESTPROG)

debug:          $(DEBTESTPROG)

normal:		$(NORMALTESTPROG)

DEBOBJ/tmpl_inst.o: factor/tmpl_inst.cc ; $(CXX) -c $< $(CFLAGS) $(DEBFLAGSTMPL) -o $@

OPTOBJ/tmpl_inst.o: factor/tmpl_inst.cc ; $(OPTCXX) -c $< $(CFLAGS) $(OPTFLAGSTMPL) -o $@


FACTORSRC1 =	factor/tmpl_inst.cc  factor/SqrFree.cc factor/Factor.cc \
		factor/MVMultiHensel.cc factor/Truefactor.cc \
		factor/helpstuff.cc 

FACTORSRC2 =	t.C

FACTORSRC = $(FACTORSRC1) $(FACTORSRC)

CHARSETSRC1 =	charset/csutil.cc charset/charset.cc 

FACTORINCL =	factor/Factor.h factor/MVMultiHensel.h factor/Truefactor.h \
		factor/tmpl_inst.h factor/SqrFree.h factor/helpstuff.h

CHARSETINCL =	charset/csutil.h charset/charset.h

OPTFACTOROBJ =	OPTOBJ/tmpl_inst.o OPTOBJ/SqrFree.o OPTOBJ/Factor.o \
		OPTOBJ/MVMultiHensel.o OPTOBJ/Truefactor.o OPTOBJ/helpstuff.o

DEBFACTOROBJ =	DEBOBJ/tmpl_inst.o DEBOBJ/SqrFree.o DEBOBJ/Factor.o \
		DEBOBJ/MVMultiHensel.o 	DEBOBJ/Truefactor.o DEBOBJ/helpstuff.o

CHARSETOPTOBJ =	OPTOBJ/csutil.o OPTOBJ/charset.o

CHARSETDEBOBJ =	DEBOBJ/csutil.o DEBOBJ/charset.o

LIBS = -lfac -lcf -lmem -lgmp -lm #-lPW
OPTLIBS = -lfac -lcf-optimize -lmem-optimize -lgmp -lm
DEBLIBS = -lfac-g -lcf-debug -lmem-debug -lgmp -lm

TESTPROG = test
TESTPROG2 = t
TESTPROG3 = test1

libfac:		$(OPTFACTOROBJ) $(CHARSETOPTOBJ)
		$(AR) $(LIBFAC) $(OPTFACTOROBJ) $(CHARSETOPTOBJ)
		$(RANLIB) $(LIBFAC)

libfac-g:	$(DEBFACTOROBJ) $(CHARSETDEBOBJ)
		$(AR) $(LIBFAC-G) $(DEBFACTOROBJ) $(CHARSETDEBOBJ)
		$(RANLIB) $(LIBFAC-G)

all:    optimize
	(cd charset; make all )

OPTOBJ/optdepend:
	../bin/makedep -P OPTOBJ  $(IFLAGS) $(FACTORSRC1)  $(CHARSETSRC1) > $@

DEBOBJ/debdepend:
	../bin/makedep -P DEBOBJ $(DEFINES) $(IFLAGS) $(FACTORSRC1) $(CHARSETSRC1) > $@

newdepend: $(FACTORINCL) $(CHARSETINCL)
	../bin/makedep -P OPTOBJ $(IFLAGS) $(FACTORSRC1) \
				> OPTOBJ/optdepend
	../bin/makedep -P DEBOBJ $(DEFINES) $(IFLAGS) $(FACTORSRC1) \
				> DEBOBJ/debdepend

OPTOBJ/test.o:	libfac test.cc $(CLAPHEADER)
	$(CXX) -c test.cc $(WARN) $(OPTFLAGS) $(IFLAGS) -o $@

DEBOBJ/test.o:	libfac-g $(DEBFACTOROBJ) test.cc $(CLAPHEADER)
	$(CXX) -c test.cc  $(DEBFLAGS) $(IFLAGS) -o $@

OPTOBJ/test1.o:	libfac test1.cc $(CLAPHEADER)
	$(CXX) -c test1.cc $(WARN) $(OPTFLAGS) $(IFLAGS) -o $@

DEBOBJ/test1.o:	libfac-g $(DEBFACTOROBJ) test1.cc $(CLAPHEADER)
	$(CXX) -c test1.cc  $(DEBFLAGS) $(IFLAGS) -o $@

opttest:	OPTOBJ/test.o 
	$(OPTLD) -o test $(LIBPATH) OPTOBJ/test.o $(OPTLIBS)

debtest:       DEBOBJ/test.o 
	$(LD) -o test $(LIBPATH) DEBOBJ/test.o $(DEBLIBS)

normaltest:	OPTOBJ/test.o
	$(OPTLD) -o test $(LIBPATH) OPTOBJ/test.o $(OPTFACTOROBJ) $(LIBS)

opttest1:	OPTOBJ/test1.o 
	$(OPTLD) -o test1 $(LIBPATH) OPTOBJ/test1.o $(OPTLIBS)

debtest1:       DEBOBJ/test1.o 
	$(LD) -o test1 $(LIBPATH) DEBOBJ/test1.o $(DEBLIBS)

normaltest1:	OPTOBJ/test1.o
	$(OPTLD) -o test1 $(LIBPATH) OPTOBJ/test1.o $(OPTFACTOROBJ) $(LIBS)

OPTOBJ/t.o:	$(OPTFACTOROBJ) t.C $(CLAPHEADER)
	$(CXX) -c t.C  $(OPTFLAGS) $(IFLAGS) -o $@

DEBOBJ/t.o:	$(DEBFACTOROBJ) t.C $(CLAPHEADER)
	$(CXX) -c t.C  $(DEBFLAGS) $(IFLAGS) -o $@


optt:	OPTOBJ/t.o
	$(OPTLD) -o t $(LIBPATH) OPTOBJ/t.o $(OPTLIBS)

debt:	DEBOBJ/t.o
	$(LD) -o t $(LIBPATH) DEBOBJ/t.o  $(DEBLIBS)

t:	OPTOBJ/t.o
	$(OPTLD) -o t $(LIBPATH) OPTOBJ/t.o $(OPTLIBS)

clean:
	$(RM)   $(TESTPROG) $(TESTPROG2) $(TESTPROG3) $(LIBFAC) $(LIBFAC-G)

cleanall:	clean
	( cd charset; make cleanall )
	$(RM)   OPTOBJ/optdepend DEBOBJ/debdepend  \
		DEBOBJ/*.o OPTOBJ/*.o \

archive:	clean	
		(cd charset ; make clean )
	rm -f ../cxxf.cpz

	gfind .	-path './OPTOBJ' -prune -o \
		-path './DEBOBJ' -prune -o \
		-path './RCS' -prune -o \
		-path './factor/RCS' -prune -o \
		-path './charset/RCS' -prune -o \
		-path './charset/DEBOBJ' -prune -o \
		-path './charset/OPTOBJ' -prune -o \
		-path './charset.orig' -prune -o \
		-path './charset.25.5.96' -prune -o \
		-print | cpio -o -a -c | gzip > ../cxxf.cpz

disk:		archive
	mcopy ../cxxf.cpz a:\cxxf.cpz
	mcopy ../cxxf.cpz a:\cxxf1.cpz

unpack:
	mcopy a:\cxxf.cpz ../cxxf.cpz
	gzcat ../cxxf.cpz | cpio -i -m -c -u -d
	( cd charset ; make dirs )

debtests:	debtest
	(cd tests ; factortest )

opttests:	opttest
	(cd tests ; factortest )

tests:	debtests

include OPTOBJ/optdepend

include DEBOBJ/debdepend

