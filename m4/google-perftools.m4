dnl
dnl    Copyright 2005-2006 Intel Corporation
dnl
dnl    Licensed under the Apache License, Version 2.0 (the "License");
dnl    you may not use this file except in compliance with the License.
dnl    You may obtain a copy of the License at
dnl
dnl        http://www.apache.org/licenses/LICENSE-2.0
dnl
dnl    Unless required by applicable law or agreed to in writing, software
dnl    distributed under the License is distributed on an "AS IS" BASIS,
dnl    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl    See the License for the specific language governing permissions and
dnl    limitations under the License.
dnl

dnl
dnl Autoconf support for linking with google's performance code
dnl

AC_DEFUN([AC_CONFIG_GOOGLE_PERFTOOLS], [

    ac_google_perfdir='no'
    AC_ARG_WITH(google-perftools,
        AS_HELP_STRING([--with-google-perftools=DIR],
            [location of a google perftools installation (default none)]),
        ac_google_perfdir=$withval)

    AC_ARG_ENABLE(google-profiling,
                  AS_HELP_STRING([--enable-google-profiling],
		  [compile with google profiling]),
                  [google_profile=$enableval],
                  [google_profile=no])

    AC_MSG_CHECKING([whether to compile with google profiling])
    AC_MSG_RESULT($google_profile)

    GOOGLE_PROFILE_ENABLED=0
    if test $google_profile = yes ; then
        if test $ac_google_perfdir = no ; then
	   AC_MSG_ERROR([must specify --with-google-perftools to use google profiling])
	fi
        GOOGLE_PROFILE_ENABLED=1
    fi

    AC_DEFINE_UNQUOTED(GOOGLE_PROFILE_ENABLED, $GOOGLE_PROFILE_ENABLED,
	 [whether google profiling support is enabled])

    dnl
    dnl First make sure we even want it
    dnl
    if test "$ac_google_perfdir" = no; then
    GOOGLE_PERFTOOLS_ENABLED=0
    else

    GOOGLE_PERFTOOLS_ENABLED=1
    AC_DEFINE_UNQUOTED(GOOGLE_PERFTOOLS_ENABLED, 1,
	 [whether google perftools support is enabled])

    dnl
    dnl Now check if we have a cached value, unless the user specified
    dnl something explicit with the --with-google-perf= argument, in
    dnl which case we force it to redo the checks (i.e. ignore the
    dnl cached values)
    dnl
    if test "$ac_google_perfdir" = yes -a \
	    ! "$oasys_cv_path_google_perf_h" = "" ; then
        echo "checking for google_perf installation... (cached)"
    else
        AC_OASYS_FIND_GOOGLE_PERFTOOLS
    fi

    if test ! $oasys_cv_path_google_perf_h = /usr/include ; then
        GOOGLE_PERFTOOLS_CFLAGS="-I$oasys_cv_path_google_perf_h"
    fi

    if test ! $oasys_cv_path_google_perf_lib = /usr/lib ; then
        GOOGLE_PERFTOOLS_LDFLAGS="-L$oasys_cv_path_google_perf_lib"
    fi

    GOOGLE_PERFTOOL_LDFLAGS="$GOOGLE_PERFTOOLS_LDFLAGS -Wl,-Bstatic -lprofiler -Wl,-Bdynamic"

    fi # GOOGLE_PERF_ENABLED
])

AC_DEFUN([AC_OASYS_FIND_GOOGLE_PERFTOOLS], [
    oasys_cv_path_google_perf_h=
    oasys_cv_path_google_perf_lib=

    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_LDFLAGS="$LDFLAGS"
    ac_save_LIBS="$LIBS"

    AC_LANG_PUSH(C++)

    if test "$ac_google_perfdir" = system -o \
            "$ac_google_perfdir" = yes -o \
            "$ac_google_perfdir" = "" ;
    then
        ac_google_perfincdirs="/usr/local/include \
	                       /usr/local/google-perftools/include"
        ac_google_perflibdirs="/usr/local/lib \
	                       /usr/local/google-perftools/lib"
    else
        ac_google_perfincdirs="$ac_google_perfdir/include"
        ac_google_perflibdirs="$ac_google_perfdir/lib"
    fi

    for ac_google_perfincdir in $ac_google_perfincdirs; do

	CPPFLAGS="$ac_save_CPPFLAGS -I$ac_google_perfincdir"
	LDFLAGS="$ac_save_LDFLAGS"
	LIBS="$ac_save_LIBS"

	dnl
	dnl First check the version in the header file. If there's a match,
	dnl fall through to the other check to make sure it links.
	dnl If not, then we can break out of the two inner loops.
	dnl
        AC_MSG_CHECKING([for google perftools header profiler.h in $ac_google_perfincdir])
	AC_LINK_IFELSE(
	  [AC_LANG_PROGRAM(
	    [
                #include <google/profiler.h>
            ],

            [
            ])],
          [
	      AC_MSG_RESULT([yes])
              oasys_cv_path_google_perf_h=$ac_google_perfincdir
	      break
          ],
          [
              AC_MSG_RESULT([no])
	      continue
          ])
    done

    AC_LANG_POP(C++)

    if test x$oasys_cv_path_google_perf_h = x ; then
        AC_MSG_ERROR([can't find usable google perftools installation])
    fi

    AC_LANG_PUSH(C++)

    for ac_google_perflibdir in $ac_google_perflibdirs; do

	LDFLAGS="$ac_save_LDFLAGS -L$ac_google_perflibdir"
        LIBS="$ac_save_LIBS -lprofiler"

        AC_MSG_CHECKING([for google perftolos library libprofiler in $ac_google_perflibdir])
	AC_LINK_IFELSE(
	  [AC_LANG_PROGRAM(
	    [
                #include <google/profiler.h>
            ],

            [
		ProfilerStart("test");
            ])],

          [
              AC_MSG_RESULT([yes])
              oasys_cv_path_google_perf_lib=$ac_google_perflibdir
              break
          ],
          [
              AC_MSG_RESULT([no])
          ])
    done

    CPPFLAGS="$ac_save_CPPFLAGS"
    LDFLAGS="$ac_save_LDFLAGS"
    LIBS="$ac_save_LIBS"

    AC_LANG_POP(C++)

    if test x$oasys_cv_path_google_perf_lib = x ; then
        AC_MSG_ERROR([can't find usable google perftools library])
    fi
])
