# Check for readline

AC_DEFUN([SING_CHECK_READLINE],
[

#AC_ARG_WITH([readline],
#  [AS_HELP_STRING([--without-readline],
#    [disable support for readline])],
#  [],
#  [with_readline=yes])


AC_CHECK_HEADERS(sys/file.h sys/ioctl.h sys/time.h sys/times.h sys/types.h \
 sys/stat.h fcntl.h sys/param.h pwd.h asm/sigcontext.h pwd.h termcap.h \
 termios.h term.h readline/readline.h)

AC_ARG_WITH(
  readline,
  [  --with-readline=[dynamic,static,no,ncurses]
                        use dynamic/static/no readline/ncurses for fancy display])

if test "$with_readline" != no; then
 AC_CHECK_LIB(ncurses,tgetent,,\
  AC_CHECK_LIB(curses,tgetent,,\
   AC_CHECK_LIB(termcap,tgetent)))
fi

# readline
if test "$with_readline" = dynamic && test "$ac_have_dl" != yes; then
  AC_MSG_WARN(can not build dynamic readline without dynamic linking)
  with_readline=static
fi


if test "$with_readline" != dynamic && test "$with_readline" != no; then
AC_LANG_SAVE
AC_LANG([C++])
   AC_CHECK_LIB(readline, readline)
   AC_CHECK_HEADERS(readline/readline.h readline/history.h)
   if test "$ac_cv_lib_readline_readline" = yes && \
      test "$ac_cv_header_readline_readline_h" = yes; then
     AC_MSG_CHECKING(whether readline.h is ok)
     AC_CACHE_VAL(ac_cv_header_readline_readline_h_ok,
     AC_TRY_LINK(
#include<unistd.h>
#include<stdio.h>
#include<readline/readline.h>
#ifdef HAVE_READLINE_HISTORY_H
#include<readline/history.h>
#endif
,
,
ac_cv_header_readline_readline_h_ok="yes",
ac_cv_header_readline_readline_h_ok="no",
))
    AC_MSG_RESULT($ac_cv_header_readline_readline_h_ok)
    if test "$ac_cv_header_readline_readline_h_ok" != yes; then
#not ok -- try once more with explicitly declaring everything
      AC_MSG_CHECKING(whether or not we nevertheless can use readline)
      AC_CACHE_VAL(ac_cv_have_readline,
      AC_TRY_LINK(
#include <stdio.h>
extern "C"
{
extern char * rl_readline_name;
extern char *rl_line_buffer;
char *filename_completion_function();
typedef char **CPPFunction ();
extern char ** completion_matches ();
extern CPPFunction * rl_attempted_completion_function;
extern FILE * rl_outstream;
char * readline ();
void add_history ();
int write_history ();
int read_history();
}
#ifndef NULL
#define NULL 0
#endif
,
rl_readline_name=NULL;
*rl_line_buffer=1;
completion_matches(NULL, filename_completion_function);
rl_attempted_completion_function = (CPPFunction *) NULL;
rl_outstream=NULL;
readline(NULL);
add_history(NULL);
read_history(NULL);
write_history(NULL);
,
ac_cv_have_readline="yes"
,
ac_cv_have_readline="no"
))
      AC_MSG_RESULT($ac_cv_have_readline)
    else
      AC_DEFINE([READLINE_READLINE_H_OK], [1], [Use readline.h])
      ac_cv_have_readline="yes"
    fi
  fi
  if test "$ac_cv_have_readline" = yes; then
    AC_DEFINE([HAVE_READLINE], [1], [Use readline])
  fi
AC_LANG_RESTORE
fi

AC_MSG_CHECKING(which readline to use)
if test "$ac_cv_with_readline" = dynamic; then
  AC_MSG_RESULT(dynamic)
  AC_DEFINE([HAVE_DYN_RL], [1], [Use dynamic readline])
elif test "$ac_cv_have_readline" = yes; then
  AC_MSG_RESULT(static)
elif test "$ac_cv_singuname" = PowerMacintosh-darwin; then
  AC_MSG_ERROR(building without readline impossible on PowerMacintosh-darwin)
else
  AC_MSG_RESULT(none)
  AC_MSG_WARN(building without readline: disabling fancy display)
fi

])
