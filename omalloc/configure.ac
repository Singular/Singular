dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl File: configure.ac
dnl Purpose: Process this file with autoconf to produce configure
dnl Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
dnl Created: 11/99
dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll

AC_INIT([omalloc],[0.9.6])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_SRCDIR(om_Alloc.c)
AC_CONFIG_HEADER([_config.h])

AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR

SING_RESET_FLAGS()

SING_WIN_FLAGS

AM_MAINTAINER_MODE([enable])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_SANITY_CHECK

# Add pre'prefixed config
AX_PREFIX_CONFIG_H([omConfig.h],[],[_config.h])

# SING_CHECK_SET_ARGS()

AC_SUBST(VERSION)

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl help for configure
dnl

AC_ARG_WITH(
  external-config_h,
  [ --with-external-config_h=HEADER_FILE
                    use HEADER_FILE for external configuration])
AC_ARG_WITH(
  external-config_c,
  [ --with-external-config_c=C_FILE
                    use C_FILE for external implementations])
AC_ARG_WITH(
  malloc,
  [ --with-malloc=system|external
                    which malloc to use, default: malloc ])
AC_ARG_WITH(
  external-malloc_h,
  [ --with-external-malloc-h=HEADER_FILE
		    use HEADER_FILE for external malloc declaration])
AC_ARG_WITH(
  external-malloc_c,
  [ --with-external-malloc-c=C_FILE
		    use C_FILE for external malloc implementation])
AC_ARG_WITH(
  valloc,
  [ --with-valloc=mmap|system|emulate
                    how to get page-aligned memory, default: use first possible])
AC_ARG_WITH(
  align,
  [ --with-align=8|sloppy|strict
                    how memory is aligned,
                    default: if possible sloppy, else strict])
AC_ARG_WITH(
  dense-bins,
  [ --with-dense-bins  use dense bin distribution])
AC_ARG_WITH(
  inline,
  [ --without-inline   do not inline])
AC_ARG_WITH(
  track,
  [ --without-track    disable debug tracking functionality])
AC_ARG_WITH(
  track-fl,
  [ --with-track-fl    track file and line numbers])
AC_ARG_WITH(
  track-return,
  [ --with-track-return track return addresses])
AC_ARG_WITH(
  track-backtrace,
  [ --with-track-backtrace track stack backtraces])
AC_ARG_WITH(
  track-custom,
  [ --with-track-custom track custom values])
AC_ARG_WITH(
  internal_debug,
  [ --with-internal-debug
                       turn on internal debugging])
AC_ARG_ENABLE(
  debug,
  [ --enable-debug    include backtrace stuff])

AC_ARG_ENABLE(omalloc,
   [AS_HELP_STRING([--disable-omalloc], [do NOT use omalloc but xalloc])],
   [if test "x$enableval"  = "xyes"; then
     ENABLE_OMALLOC=yes
     AH_TEMPLATE([HAVE_OMALLOC], [define if build with OMALLOC])
     AC_DEFINE([HAVE_OMALLOC],[1])
   else
     ENABLE_OMALLOC=no
   fi],
   [ENABLE_OMALLOC=yes
     AH_TEMPLATE([HAVE_OMALLOC], [define if build with OMALLOC])
     AC_DEFINE([HAVE_OMALLOC],[1])
   ])
dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl compiler/make  config
dnl
dnl I'm not sure why I did this
dnl if test "${CFLAGS+set}" != set; then
dnl  CFLAGS="-O"
dnl  ac_cflags_set=no
dnl fi

CPPFLAGS="-I.. -I. $CPPFLAGS"
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CPP

AC_PROG_LN_S
AC_PROG_INSTALL
AM_PROG_CC_C_O
AC_C_CONST
AC_C_INLINE

LT_INIT([win32-dll])


dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl check for programs
dnl
AC_CHECK_PROG(AR, ar, ar, :)
if test "$ac_cv_prog_AR" = no; then
  AC_MSG_ERROR(*** ar program not found)
fi
AC_CHECK_PROGS(PERL, perl, "no")
if test "$ac_cv_prog_PERL" = no; then
  AC_MSG_ERROR(*** perl program not found)
fi

AC_CHECK_PROG(ADDR2LINE, addr2line, addr2line, no)
if test "$ac_cv_prog_ADDR2LINE" = addr2line; then
  AC_DEFINE_UNQUOTED(OM_PROG_ADDR2LINE, "$ac_cv_prog_ADDR2LINE", "Name of addr2line")
fi

AC_MSG_NOTICE(HOSTCC set)
HOSTCC=${HOSTCC:=$CC}
HOSTLD=${HOSTLD:=$LD}
HOSTCFLAGS=${HOSTCFLAGS:=$CFLAGS}
  AC_SUBST(HOSTCC)
  AC_SUBST(HOSTLD)
  AC_SUBST(HOSTCFLAGS)

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl header file checks
dnl
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h,,
  AC_MSG_ERROR(Can not compile without limits.h))

AC_CHECK_HEADERS(unistd.h sys/mman.h fcntl.h malloc.h malloc/malloc.h)

AC_CHECK_FUNCS(popen mmap sbrk random malloc_usable_size malloc_size)

dnl llllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl Find out more about particularity of the system
dnl

# sizes
AC_CHECK_SIZEOF(long,4)
AC_CHECK_SIZEOF(void*,4)
AC_CHECK_SIZEOF(double, 8)
AC_CHECK_SIZEOF(size_t, 4)

if test "$ac_cv_sizeof_long" != "$ac_cv_sizeof_voidp"; then
  AC_MSG_ERROR(need equal sizes for long and void*)
fi
if test "$ac_cv_sizeof_voidp" != 4 && test "$ac_cv_sizeof_voidp" != 8; then
  AC_MSG_ERROR(need void* to be 4 or 8 bytes long)
fi
if test "$ac_cv_sizeof_double" != 4 && test "$ac_cv_sizeof_double" != 8; then
  AC_MSG_ERROR(need double to be 4 or 8 bytes long)
fi

dnl Set compiler, linker flags so that we can work with omalloc
BACKUP_CFLAGS=$CFLAGS
BACKUP_CXXFLAGS=$CXXFLAGS
BACKUP_LDFLAGS=$LDFLAGS

CFLAGS="$CFLAGS -I$srcdir"
CXXFLAGS="$CXXFLAGS -I$srcdir"

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl figure out page size of the system
dnl
AC_MSG_CHECKING(size of system page)
AC_CACHE_VAL(ac_cv_pagesize,
AC_TRY_RUN([#include <stdio.h>
#include <stdlib.h>
#include "omGetPageSize.h"

int main()
{
  FILE *f=fopen("conftestval", "w");
  if (!f) exit(1);
  fprintf(f, "%d\n", (int)omalloc_getpagesize);
  exit(0);
}], ac_cv_pagesize=`cat conftestval`, ac_cv_pagesize=0, ac_cv_pagesize=0))
# can not really handle pagesizes which are greater -- there must be a
# bug somewhere
if test $ac_cv_pagesize -gt 8192; then
  ac_cv_pagesize=8192
fi
AC_MSG_RESULT($ac_cv_pagesize)
if test "$ac_cv_pagesize" = 4096 || test "$ac_cv_pagesize" = 8192; then
  AC_DEFINE_UNQUOTED(SIZEOF_SYSTEM_PAGE, $ac_cv_pagesize, "Page-size of the build-system")
else
AC_MSG_ERROR([need sytem page to be of size 4096 or 8192, but is $ac_cv_pagesize])
fi

AC_DEFINE_UNQUOTED(SIZEOF_OM_PAGE,SIZEOF_SYSTEM_PAGE,Page-size of the build-system)

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl check whether mmap actually works
dnl
if test "$ac_cv_func_mmap" = yes; then
AC_MSG_CHECKING(whether mmap works)
AC_CACHE_VAL(ac_cv_working_mmap,
AC_TRY_RUN([
#include <stdlib.h>
#include "omMmap.c"
int main()
{
  void* addr = omVallocMmap(128*${ac_cv_pagesize});
  if (addr == 0 || ((unsigned long) addr % ${ac_cv_pagesize}))
    exit(1);
  exit(omVfreeMmap(addr, 128*${ac_cv_pagesize}));
}], ac_cv_working_mmap=yes, ac_cv_working_mmap=no, ac_cv_working_mmap=no))
AC_MSG_RESULT($ac_cv_working_mmap)
if test "$ac_cv_working_mmap" = yes; then
  AC_DEFINE(HAVE_WORKING_MMAP,1,"Whether we have a working mmap")
fi
fi

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl for strict alignment
dnl
AC_CACHE_CHECK(whether alignment needs to be strict, ac_cv_align_need_strict,
AC_TRY_RUN([
#include <stdlib.h>
int main()
{
  void* ptr = (void*) malloc(12);
  volatile double* d_ptr;
  if ((unsigned long) ptr % 8 == 0) ptr = ptr + 4;
  d_ptr = (double*) ptr;
  *d_ptr = (double) 1.25;
  if (*d_ptr != (double) 1.25) exit(1);
  else exit(0);
}
], ac_cv_align_need_strict=no, ac_cv_align_need_strict=yes, ac_cv_align_need_strict=yes))

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl external config files
dnl
AC_MSG_CHECKING(for external config files)
if test "${with_external_config_h+set}" = set; then
    AC_DEFINE(OM_HAVE_EXTERNAL_CONFIG_H,1,"Whether we have external config.h")
    EXTERNAL_CONFIG_HEADER=${with_external_config_h}
    rm -f omExternalConfig.h
    cp ${with_external_config_h} omExternalConfig.h
fi
if test "${with_external_config_c+set}" = set; then
    AC_DEFINE(OM_HAVE_EXTERNAL_CONFIG_C,1,"Wether we have an external config.c")
    EXTERNAL_CONFIG_SOURCE=${with_external_config_c}
fi
AC_SUBST(EXTERNAL_CONFIG_HEADER)
AC_SUBST(EXTERNAL_CONFIG_SOURCE)
if test "${EXTERNAL_CONFIG_HEADER+set}" = set || test "${EXTERNAL_CONFIG_SOURCE+set}" = set; then
AC_MSG_RESULT(${EXTERNAL_CONFIG_HEADER} ${EXTERNAL_CONFIG_SOURCE})
else
AC_MSG_RESULT(none)
fi

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl for malloc
dnl
AC_MSG_CHECKING(which malloc to use)
if test "${with_malloc}" = system; then
  OM_MALLOC_HEADER=omMallocSystem.h
  AC_DEFINE(OMALLOC_USES_MALLOC,1,the system allocator is called malloc)
  AC_DEFINE(OMALLOC_USES_SYSTEM_MALLOC,1,use system malloc as system allocator)
elif test "${with_malloc}" = external; then
  if test "${with_external_malloc_h+set}" != set; then
    AC_MSG_ERROR(need --with_external_malloc_h for external malloc)
  fi
  AC_DEFINE(OMALLOC_USES_EXTERNAL_MALLOC,1,use external malloc as system allocator)
  OM_MALLOC_HEADER=$with_external_malloc_h
  OM_MALLOC_SOURCE=$with_external_malloc_c
  AC_DEFINE(OMALLOC_USES_MALLOC,1,the system allocator is called malloc)
else	## default
  OM_MALLOC_HEADER=omMallocSystem.h
  AC_DEFINE(OMALLOC_USES_MALLOC,1,the system allocator is called malloc)
  AC_DEFINE(OMALLOC_USES_SYSTEM_MALLOC,1,use system malloc as system allocator)
fi
AC_MSG_RESULT($with_malloc)

AC_SUBST(OM_MALLOC_HEADER)
AC_SUBST(OM_MALLOC_SOURCE)
if test "${OM_MALLOC_SOURCE+set}" = set; then
  AC_DEFINE(OM_HAVE_MALLOC_SOURCE,1,"Whether we have the source for malloc ()")
fi

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl check whether valloc is provided and that it works
dnl
AC_MSG_CHECKING(whether working valloc exists)
if test "${ac_cv_working_valloc}" = no; then
  AC_MSG_RESULT( (cached) no)
elif test "${ac_cv_working_valloc}" = "${OM_MALLOC_HEADER}_${OM_MALLOC_SOURCE}"; then
  AC_MSG_RESULT( (cached) yes)
else
AC_TRY_RUN([
#include <stdlib.h>
#include "$OM_MALLOC_HEADER"
#ifdef OM_HAVE_MALLOC_SOURCE
#include "$OM_MALLOC_SOURCE"
#endif

int main()
{
  void* addr = OM_MALLOC_VALLOC(128*${ac_cv_pagesize});
  if (addr == 0 || ((unsigned long) addr % ${ac_cv_pagesize}))
    exit(1);
  OM_MALLOC_VFREE(addr, 128*${ac_cv_pagesize});
  exit(0);
}
], ac_cv_working_valloc="${OM_MALLOC_HEADER}_${OM_MALLOC_SOURCE}", ac_cv_working_valloc=no, ac_cv_working_valloc=no)
if test "${ac_cv_working_valloc}" = no; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
fi
fi

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl for valloc
dnl
AC_MSG_CHECKING(which valloc to use)
if test "${with_valloc+set}" != set || test "${with_valloc}" = mmap; then
  if test "${ac_cv_working_mmap}" = yes; then
    with_valloc=mmap
    AC_DEFINE(OM_HAVE_VALLOC_MMAP,1,"Have valloc")
  else
    with_valloc=malloc
  fi
fi
if test "${with_valloc}" = malloc; then
  if test "${ac_cv_working_valloc}" != no; then
    AC_DEFINE(OM_HAVE_VALLOC_MALLOC,1,"Have valloc")
  else
    with_valloc=emulate
  fi
fi
AC_MSG_RESULT($with_valloc)

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl alignment
dnl
AC_MSG_CHECKING(how to align)
if test "$with_align" = 8 || test "$ac_cv_sizeof_long" = 8; then
  ac_cv_align=8
  AC_DEFINE(OM_ALIGN_8,1,"Align to 8 bytes")
else
if test "$ac_cv_align_need_strict" = "yes" || test "$with_align" = "strict"; then
  AC_DEFINE(OM_ALIGNMENT_NEEDS_WORK,1,"Whether alignment needs work")
  ac_cv_align="strict"
else
  ac_cv_align="sloppy"
fi
fi
AC_MSG_RESULT($ac_cv_align)

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl dense bins
dnl
AC_MSG_CHECKING(whether to use dense bins)
if test "$with_dense_bins" = yes; then
AC_MSG_RESULT(yes)
AC_DEFINE(OM_HAVE_DENSE_BIN_DISTRIBUTION,1,"Whether we have dense bins")
else
AC_MSG_RESULT(no)
fi

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl debug and inline
dnl
save_cflags="$CFLAGS"
AC_MSG_CHECKING(whether to disable debugging)
if test "$enable_debug" = yes; then
AC_MSG_RESULT(no)
CFLAGS="$CFLAGS -g"
else
AC_MSG_RESULT(yes)
AC_DEFINE(OM_NDEBUG,1,"Disable debug")
with_track=no
fi
AM_CONDITIONAL([WANT_DEBUG],[test x"$with_debug" = xyes])

AC_MSG_CHECKING(whether to have tracking debug functionality)
if test "$with_track" != no; then
AC_MSG_RESULT(yes)
AC_DEFINE(OM_HAVE_TRACK,1,"Have track")
else
AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to use internal debug)
if test "$with_internal_debug" = yes; then
AC_MSG_RESULT(yes)
AC_DEFINE(OM_INTERNAL_DEBUG,1,"Internal debug")
with_inline=no
else
AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to inline)
if test "$ac_cv_c_inline" != no && test "$with_inline" != no; then
  AC_DEFINE_UNQUOTED(OM_INLINE, static $ac_cv_c_inline, "inline-declaration")
  AC_DEFINE_UNQUOTED(OM_INLINE_DECL, static $ac_cv_c_inline, "inline-declaration")
  AC_DEFINE_UNQUOTED(OM_INLINE_IMPL, static $ac_cv_c_inline, "inline-declaration")
  AC_DEFINE_UNQUOTED(OM_INLINE_LOCAL, static $ac_cv_c_inline, "inline-declaration")
  AC_MSG_RESULT(yes)
else
  AC_DEFINE_UNQUOTED(OM_INLINE_DECL, extern, "inline-declaration")
  AC_DEFINE_UNQUOTED(OM_INLINE_IMPL,, "inline-declaration")
  AC_DEFINE_UNQUOTED(OM_INLINE_LOCAL, static, "inline-declaration")
  AC_MSG_RESULT(no)
fi

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl backtrace business
dnl
if test "x$with_track" != xno; then
AC_MSG_CHECKING(whether GET_RET_ADDR works)
AC_CACHE_VAL(ac_cv_get_return_addr_works,
AC_TRY_RUN([
#include <stdlib.h>
#define OM_GET_RETURN_ADDR_WORKS 1
#include "omReturn.h"
int test_return_addr()
{
  char* f;
  GET_RET_ADDR(f);
  return (int)(long) f;
}
int main()
{
   exit(! test_return_addr());
}
], ac_cv_get_return_addr_works=yes,
   ac_cv_get_return_addr_works=no,
   ac_cv_get_return_addr_works=no))
AC_MSG_RESULT($ac_cv_get_return_addr_works)
if test "$ac_cv_get_return_addr_works" = yes; then
AC_DEFINE(OM_GET_RETURN_ADDR_WORKS,1, "Whether get return address works")
fi

AC_MSG_CHECKING(whether omGetBackTrace works)
AC_CACHE_VAL(ac_cv_get_backtrace_works,
AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include "omGetBackTrace.c"
int test_backtrace0()
{
  void* bt @<:@ 10 @:>@ ;
  int i = omGetBackTrace(bt, 0, 10);
  return i;
}
int test_backtrace(){ return test_backtrace0(); }
int main()
{
   int i;
   #ifdef __APPLE__
   /* omGetBackTrace crashes on OsX for i>2 */
   i=0;
   #else
   omInitGetBackTrace();
   i = test_backtrace();
   #endif
   if (i > 0) exit(0);
   else exit(i+10);
}
], ac_cv_get_backtrace_works=yes,
   ac_cv_get_backtrace_works=no,
   ac_cv_get_backtrace_works=no))
AC_MSG_RESULT($ac_cv_get_backtrace_works)
if test "$ac_cv_get_backtrace_works" = yes; then
AC_DEFINE(OM_GET_BACKTRACE_WORKS,1,"Whether omInitGetBackTrace () works")
fi
else
   AC_MSG_CHECKING(whether GET_RET_ADDR works)
   ac_cv_get_return_addr_works=no
   AC_MSG_RESULT($ac_cv_get_return_addr_works)
   AC_MSG_CHECKING(whether omGetBackTrace works)
   ac_cv_get_backtrace_works=no
   AC_MSG_RESULT($ac_cv_get_backtrace_works)
fi

AC_MSG_CHECKING(whether addr2line works)
AC_CACHE_VAL(ac_cv_prog_addr2line_works,
AC_TRY_RUN([
#include <stdlib.h>
#define OM_GET_RETURN_ADDR_WORKS 1
#include "omReturn.h"
#include "omStructs.h"
/* #include "omGetBackTrace.h" */
#include "omGetBackTrace.c"
#include "omRet2Info.c"

int test_Ret_2_Info()
{
  void* bt;
  int i;
  struct omRetInfo_s info;

  GET_RET_ADDR(bt);
  i = omBackTrace_2_RetInfo(&bt, &info, 1);
  return i;
}

int main(int argc, char** argv)
{
  int i;
  omInitRet_2_Info(*argv);
  i = test_Ret_2_Info();
  if (i==1) exit(0);
  else exit (i+10);
}
], ac_cv_prog_addr2line_works=yes,
   ac_cv_prog_addr2line_works=no,
   ac_cv_prog_addr2line_works=no))
AC_MSG_RESULT($ac_cv_prog_addr2line_works)

CFLAGS="$save_cflags"

AC_MSG_CHECKING(whether to track return addresses)
if test "$with_track_return" = no || test "$ac_cv_get_return_addr_works" = no || test "$ac_cv_prog_addr2line_works" != yes; then
  with_track_return=no
else
  AC_DEFINE(OM_TRACK_RETURN,1,"Whether to track return")
  with_track_return=yes
fi
AC_MSG_RESULT($with_track_return)

AC_MSG_CHECKING(whether to track files and line numbers)
if test "$with_track_fl" = no && test "$with_track_return" = no; then
  with_track_fl=yes
fi
if test "${with_track_fl+set}" != set; then
  if test "$with_track_return" = yes; then
    with_track_fl=no
  else
    with_track_fl=yes
  fi
fi
AC_MSG_RESULT($with_track_fl)
if test "$with_track_fl" = yes; then
  AC_DEFINE(OM_TRACK_FILE_LINE,1,"Whether to track file-line")
fi

AC_MSG_CHECKING(whether to track stack backtraces)
if test "$with_track" != no && test "$ac_cv_get_backtrace_works" = yes && test "$with_track_backtrace" != no && test "$ac_cv_prog_addr2line_works" = yes; then
  with_track_backtrace=yes
  AC_DEFINE(OM_TRACK_BACKTRACE,1,"Whether to track backtrace")
else
  with_track_backtrace=no
fi
AC_MSG_RESULT($with_track_backtrace)

AC_MSG_CHECKING(whether to track custom values)
if test "$with_track" != no && test "$with_track_custom" = yes; then
  AC_DEFINE(OM_TRACK_CUSTOM,1,"Enable custom tracking")
else
  with_track_custom=no
fi
AC_MSG_RESULT($with_track_custom)

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl figure out the return type of sbrk
dnl

AC_MSG_CHECKING(return type of sbrk)
AC_TRY_LINK(
#define __USE_MISC
#include <unistd.h>
,
void *sbrk();
,
AC_DEFINE(Void_t,void,"Type of void"),
AC_DEFINE(Void_t,char,"Type of void"),
)
AC_MSG_RESULT(Void_t)


AC_DEFINE(OM_PROVIDE_MALLOC,0,[Provide NO standard routines!])


dnl Restore user-specified CFLAGS, CXXFLAGS, LIBS

CFLAGS=$BACKUP_CFLAGS
CXXFLAGS=$BACKUP_CXXFLAGS
LDFLAGS=$BACKUP_LDFLAGS

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl wrap it up
dnl
AC_CONFIG_FILES([Makefile omalloc.pc])
AC_OUTPUT

