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
        AC_ARG_WITH(
            python,
            AS_HELP_STRING([--with-python@<:@=PYTHON@:>@],
                [absolute path name of Python executable]
            ),
            [],[withval="yes"]
        ) 
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
                AC_PATH_PROG(
                    [PYTHON],
                    python,
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
                                        [ ax_python_use=true
                                          AC_MSG_RESULT(yes)
                                          AX_PYTHON_PREFIX( )
                                          AX_PYTHON_LSPEC( )
                                          AX_PYTHON_CSPEC( )
                                        ],
                                        [ax_python_use=false; AC_MSG_RESULT([too old, skipping python interface!])]
                )
            fi
        fi   
        AM_CONDITIONAL(PYTHON_USE, test x"$ax_python_use" = x"true")
    fi

])

