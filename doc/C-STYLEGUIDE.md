Legacy C/C++-Style guide for Singular {#legacystyleguide}
=====================================

Please also note the [newer C/C++ programming style guide](@ref styleguide)
and the [Singular build system description](@ref bs_page).

[TOC]


## C/C++ files - names (applies to headers as well)
  - each C++ file should have the extension .cc
  - each C file should have the extension .c
  - each header file should have the extension .h
    it should be possible to include it in C and C++ sources
  - converted to lower case, each file name must be unique in the
    first 8 characters
  - it is recommended to use only lower case file names

## C/C++ files - structure
  - TODO: What about Copyright note?
    (Contained in COPYING unless otherwise specified).
  - each C/C++ file (abc.c/abc.cc/abc.h) should start with a short comment
    about its purpose in doxygen format (TODO: see file templates) giving
    at least the filename and a brief description of the contents.
    (Each header of a file should contain an Id/Rev field)
  - order "#include" statements from global to local scope:
      i. first: all system include files:
        #include <iostream>, #include <boost/shred_ptr.h>
       (remember to include optional include files in #ifdef ... #end)
     ii. only source files: #include "mod2.h"
    iii. at last: all other files (which you really need):
        #include "poly.h" etc.
  - paths in "#include" statements should be avoided (should be specified by
    build system level instead)

## Header files
  - see C/C++ files - structure!
  - each header file abc.h should contain a multiple inclusions preventing
    mechanism as follows:

    #ifndef ABC_H
    #define ABC_H

    // .... declarations ...

    #endif // ABC_H

  - do NOT include mod2.h!
  - include only the header files you really need
    (use forward declarations wherever possible)
  - all declarations (macros, types, variables, enumerations, function
    parameters...) should be documented in doxygen format. Brief comments are
    mandatory, long comments are optional (TODO: see file templates)
  - class-/function-/member variable-/... comments should be written in the
    doxygen format (see Doxygen quick reference card)
  - further (non doxygen) comments can be used to separate the file into easily
    visible sections

## format
  - general screen width: 80 columns
  - each procedure declarations should be in one line, if possible
  - the number of required function parameters should be as small as possible
  - curly braces: Matching curly brackets should be either vertically
    or horizontally aligned.
  - the "else" keyword indents the same as its matching "if"
  - indentation should be small
    (e.g. two positions (spaces) for each level of nesting)
  - avoid tabs: their interpretation differs from editor to editor.
  - use empty lines seldom: only to break up very long routines and between
    routines
  - avoid code copying/duplication
  - declare local variables as late as possible and with the smallest possible
    visibility scope
  - avoid using "goto", "continue", "break" statements
    (save for "switch/case" blocks and error handling)
  - compiler warnings should be enabled and regarded as errors
  - whenever possible, constants should be defined as "const variables"
    not via "#define".
    Non pure C++ parts must use #define.
  - Consider the choice between macros and inline function very careful,
    prefer inline functions:
    - macros are not type safe
    + macros are always inlined
    - arguments to macros can be multiply computed

    - "inline" is only a hint for the optimizer (especially in the C parts):
      in non-optimzed code these functions are NOT inlined.
    -/+ inline functions are not generic (requires the defined types)

## Naming conventions:
  - All code and global variables must conform to this naming convention,
    it does not apply to local variables.
  - the names consists of a short (small letter) prefix,
    the first letter of each following word is capitalized
    (The routines Werror/WerrorS/Warn/WarnS/Print/PrintS have an empty prefix)
  - the prefix describes the area the name belongs to:
    p: polynomial operations
    n/np/nl/na/: number operations (general/Zp/Q/alg.ext.)
    k: std engine
    ...
  - _ (underscore) is only used as a last part of the prefix:
    (example: p_Add): the last argument is the base ring
  - macros are in capital letters (except used as a procedure)



## Comments
  - class-/function-/member variable-/... comments should be written in the
    doxygen format (see Doxygen quick reference card)
  - documentation should explain the purpose of each type/function/parameter/.,
    as well as its return value or any preconditions/side effects if applicable
  - comments in source files about implementation details need not be in
    doxygen format
  - do not comment on trivial matters. Implementation details should be
    sufficiently commented for others to understand the inner workings of the
    code.
  - document difficult algorithms by a reference to an article/book/etc.

## Checks / Debugging aids
  - interpreter routines have to check their input
  - use const wherever possible/suitable (especially in declarations of input
    parameters to functions/methods provided as pointers or references or for
    methods that do not change the state of an object, consider declaring
    variables "mutable" whenever suitable)

  - kernel routines have to document their requirements,
    the checks have to be done in the interpreter.
    Repeat the checks only within #ifndef SING_NDEBUG/#endif
    or via assert.
  - input should be checked to conform with the documentation via assume
    (if this is simple)
  - more time consuming tests should be within
    #ifdef PDEBUG/#ifdef KDEBUG/#ifdef LDEBUG
    (see mod2.h)
  - case statements (or equivalent if/else constructs)
    should always have an default entry
    (maybe an error message)

## C++ features
  - to avoid confusion: "struct" should be a C object,
    if you really need C++ extensions, use "class".
  - "and"/"or"/"not" are not recognized by all compilers, use &&, &, ||, |, !
