#
# GNUmakefile - `GNUmakefile' to make some sample applications of
#   Factory.
#
# Note: This `GNUmakefile' is not configured by `configure'.  To
# configure it, change the variables at the beginning of the
# file.
#

#
# - paths.
#

# where to look for GMP and Factory
GMPPREFIX=/usr/local
FACTORYPREFIX=$(HOME)/factory

#
# - flags.
#

# compiler
CXX=g++

# compiler and linker options.  Configured for an optimized
# version of Factory.
WARNFLAGS=-w
DEFS=
CPPFLAGS=
CFLAGS=-O2 -fomit-frame-pointer
CXXFLAGS=-O2 -fomit-frame-pointer
LIBS=-L$(FACTORYPREFIX)/lib -lcf -lcfmem -L$(GMPPREFIX)/lib -lgmp -lm
LDFLAGS=

#
# - compiler flags.
#

# flags to translate ftmpl_inst.cc.  We allow implicit template
# instantiations in this case.
TMPLCXXFLAGS =	$(WARNFLAGS) \
		-I$(FACTORYPREFIX)/include \
		$(DEFS) $(CPPFLAGS) $(CXXFLAGS)

# flags to translate application
APPLCXXFLAGS =	$(WARNFLAGS) -fno-implicit-templates \
		-I$(FACTORYPREFIX)/include \
		$(DEFS) $(CPPFLAGS) $(CXXFLAGS)

# flags to link application
APPLLDFLAGS =	$(LIBS) $(LDFLAGS)

#
# - files.
#
applications := \
		application \
		factorize \
		gcd

#
# - phony targets.
#
.PHONY:		all clean

#
# - pattern rules.
#
%.o:		%.cc
		$(CXX) -c $< $(APPLCXXFLAGS) -o $@

#
# - explicit targets.
#

# compile all applications
all:		$(applications)

ftmpl_inst.o:	$(FACTORYPREFIX)/include/templates/ftmpl_inst.cc
		$(CXX) -c $< $(TMPLCXXFLAGS) -o $@

application:	application.o ftmpl_inst.o
		$(CXX) $^ $(APPLLDFLAGS) -o $@
		strip $@

factorize:	factorize.o ftmpl_inst.o
		$(CXX) $^ $(APPLLDFLAGS) -o $@
		strip $@

gcd:		gcd.o ftmpl_inst.o
		$(CXX) $^ $(APPLLDFLAGS) -o $@
		strip $@

# clean up this directory
clean:
		rm -f *.o $(applications)
