NODEBUG = 1

!include <ntwin32.mak>

objs =	"canonicalform.obj" \
	"cf_algorithm.obj" \
	"cf_binom.obj" \
	"cf_char.obj" \
	"cf_chinese.obj" \
	"cf_eval.obj" \
	"cf_factor.obj" \
	"cf_factory.obj" \
	"cf_gcd.obj" \
	"cf_generator.obj" \
	"cf_globals.obj" \
	"cf_irred.obj" \
	"cf_iter.obj" \
	"cf_linsys.obj" \
	"cf_map.obj" \
	"cf_ops.obj" \
	"cf_primes.obj" \
	"cf_random.obj" \
	"cf_resultant.obj" \
	"cf_reval.obj" \
	"cf_switches.obj" \
	"cf_util.obj" \
	"debug.obj" \
	"fac_berlekamp.obj" \
	"fac_cantzass.obj" \
	"fac_distrib.obj" \
	"fac_ezgcd.obj" \
	"fac_iterfor.obj" \
	"fac_multihensel.obj" \
	"fac_multivar.obj" \
	"fac_sqrfree.obj" \
	"fac_univar.obj" \
	"fac_util.obj" \
	"ffops.obj" \
	"ftmpl_inst.obj" \
	"gfops.obj" \
	"gf_tabutil.obj" \
	"initgmp.obj" \
	"int_cf.obj" \
	"int_int.obj" \
	"int_poly.obj" \
	"int_pp.obj" \
	"int_rat.obj" \
	"newdelete.obj" \
	"parseutil.obj" \
	"readcf.obj" \
	"sm_sparsemod.obj" \
	"sm_util.obj" \
	"variable.obj" \
	"memutil.obj"


INCLUDE = .;$(INCLUDE)
LIB = .;$(LIB)

cpp = cl
cflags = -W3 -DWINNT -D__STDC__ -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -nologo -D_X86_=1 -DWINVER=0x400 
cppflags = $(cflags) $(cdebug) $(cvarsmt) /GX /MT

all:	factory.lib

.SUFFIXES: .cc .obj .c .h .exe


.c.obj:
	$(cpp) $(cppflags) /c $*.c


.cc.obj:
	$(cpp) /TP $(cppflags) /c $*.cc

factory.lib: $(objs)
	$(implib) -out:factory.lib $(objs)

testfac.exe: testfac.obj factory.lib
	$(link) /nodefaultlib:libc $(linkdebug) -out:testfac.exe testfac.obj factory.lib gmp.lib $(conlibsmt)

