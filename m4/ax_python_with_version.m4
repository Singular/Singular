# AX_PYTHON_WITH_VERSION( [version], [path] )
# -----------------------------------------------------------------
# Handles the various --with-python commands.
# Input:
#   $1 is the minimun required python version
#   $2 is the optional search path for the python executable if needed
# Ouput:
#   PYTHON_USE (AM_CONDITIONAL) is true if python executable found
#   and --with-python was requested; otherwise false.
#   $PYTHON contains the full executable path to python if PYTHON_USE
#   is true.
#
# Example:
#   AX_PYTHON_WITH_VERSION([2.4])
#   or
#   AX_PYTHON_WITH_VERSION([2.4], "/usr/bin")

AC_DEFUN([AX_PYTHON_WITH_VERSION],
[
    AC_ARG_VAR([PYTHON],[Python Executable Path])

    # unless PYTHON was supplied to us (as a precious variable),
    # see if --with-python[=PythonExecutablePath], --with-python,
    # --without-python or --with-python=no was given.
    if test -z "$PYTHON"
    then
        AC_MSG_CHECKING(for --with-python)
# @<:@=@<:@embed,@:>@PYTHON@:>@
          AC_ARG_WITH(
            python,
            AS_HELP_STRING([--with-python],
                [absolute path name of Python executable]
            ),
            [],[withval="yes"]
        ) 
        si_try_embed="no"
        py_save_ifs="$IFS"; IFS="${IFS}$PATH_SEPARATOR,"
        for elt in $withval; do
          IFS="$py_save_ifs"
          case $elt in
            embed|embedding)
              si_try_embed="yes"
            ;;
            static|dynamic|shared|module)
            ;;
            *)
            si_withval=$elt
          esac 
        done
        IFS="$py_save_ifs"
        if test x"$si_withval" = x""
        then
           withval="yes"
        else
	   withval="$si_withval"
        fi

        AC_MSG_RESULT($withval)
        if test "$withval" = "no"
        then
            ax_python_use=false
        else        
            if test "$withval" = "yes"
            then
                # "yes" was specified, but we don't have a path
                # for the executable.
                # So, let's search the PATH Environment Variable.
                AC_PATH_PROGS(
                    [PYTHON],
                    [python2.7 python2.6 python2.5 python2.4 python2.3 python2.2 python2.1 python2 python],
                    [],
                    $2
                )
            else
                # $withval must be the executable path then.
                AC_SUBST([PYTHON], ["${withval}"])
            fi
            if test -z "$PYTHON"
            then
                AC_MSG_RESULT([no path to python found, skipping python interface!])
            else 
                AX_PYTHON_VERSION_CHECK([$1],
                  [ AC_MSG_RESULT(yes)
                    AX_PYTHON_VERSION_CHECK([3],
                    [ ax_python_use=false
                      AC_MSG_RESULT([too recent, skipping python interface!])
                    ],
                    [ AC_MSG_RESULT([no (ok)])
                      ax_python_version=`$PYTHON -c "import sys; print '.'.join(sys.version.split('.')[[:2]])"`
                      AC_CHECK_LIB([python${ax_python_version}],[main], [
                          ax_python_use=true
                          si_embed_python=$si_try_embed
                          AC_MSG_CHECKING(embedding python interface module)
                          AC_MSG_RESULT($si_try_embed)
                          AX_PYTHON_PREFIX( )
                          AX_PYTHON_LSPEC( )
                          AX_PYTHON_CSPEC( )
                        ],
                        [ ax_python_use=false
                          AC_MSG_RESULT([Cannot link to python, skipping python interface!])
                        ]
                      )
                    ])
                  ],
                  [ ax_python_use=false
                    AC_MSG_RESULT([too old, skipping python interface!])]
                )
            fi
        fi   

        if  test x"$si_embed_python" = x"yes"
        then 
          AC_DEFINE(EMBED_PYTHON,1,integrate python)
#          AC_SUBST(EMBED_PYOBJECT_CFLAGS,"\${PYTHON_CSPEC}")
#          AC_SUBST(EMBED_PYOBJECT_LDFLAGS,"\${PYTHON_LSPEC}")
        fi
	if test x"$ax_python_use" = x"true" 
	then 
          AC_DEFINE(HAVE_PYTHON,1,[compile python-related stuff])
        fi
    fi
    
    AM_CONDITIONAL(PYTHON_USE, test x"$ax_python_use" = x"true")
    AM_CONDITIONAL(SI_EMBED_PYTHON, test x"$ax_python_use$si_embed_python" = x"trueyes")
    AM_CONDITIONAL(PYTHON_MODULE, test x"$ax_python_use" = x"true" -a x"$si_embed_python" != x"yes" )

])

