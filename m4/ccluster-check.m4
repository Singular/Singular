# Check for ccluster

AC_DEFUN([LB_CHECK_CCLUSTER],
[
AC_REQUIRE([SING_DEFAULT_CHECKING_PATH])

AC_ARG_WITH(ccluster,
[  --with-ccluster= <path>|yes Use ccluster library.  ],
		[if test "$withval" = yes ; then
			CCLUSTER_HOME_PATH="${DEFAULT_CHECKING_PATH} /usr"
	         elif test "$withval" != no ; then
			CCLUSTER_HOME_PATH="$withval"
	        fi],
		[CCLUSTER_HOME_PATH="${DEFAULT_CHECKING_PATH} /usr"])

dnl Check for existence
BACKUP_CFLAGS=${CFLAGS}
BACKUP_LIBS=${LIBS}

AC_MSG_CHECKING(for Ccluster)

AC_LANG_PUSH([C])

for CCLUSTER_HOME in ${CCLUSTER_HOME_PATH}
  do
	if test -r "$CCLUSTER_HOME/include/ccluster/ccluster/ccluster.h"; then
	        AC_MSG_RESULT(found)

	        CCLUSTER_CPPFLAGS="-I${CCLUSTER_HOME}/include/ccluster"
		CCLUSTER_LIBS="-L${CCLUSTER_HOME}/lib -Wl,-rpath,${CCLUSTER_HOME}/lib -lccluster"
		AC_DEFINE(HAVE_CCLUSTER,1,[Define if Ccluster is installed])

		CFLAGS="${BACKUP_CFLAGS} ${CCLUSTER_CPPFLAGS}"
		LIBS="${BACKUP_LIBS} ${CCLUSTER_LIBS}"
		AC_SUBST(CCLUSTER_LIBS)
		AC_SUBST(CCLUSTER_CPPFLAGS)
                break
	fi
done
AC_LANG_POP([C])

CFLAGS=${BACKUP_CFLAGS}
LIBS=${BACKUP_LIBS}
#unset LD_LIBRARY_PATH

if test "x$HAVE_CCLUSTER" != "xyes"; then
  AC_MSG_RESULT(not found)
fi
AM_CONDITIONAL(SING_HAVE_CCLUSTER, test "x$HAVE_CCLUSTER" = "xyes")
])
