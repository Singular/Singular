Style Guide for the Further Development of SINGULAR in C / C++ {#styleguide}
==============================================================

[TOC]

## Preface

The purpose of this document is to provide a common style guide for the further code development for SINGULAR in C and C++. Observation of this style guide will

 * increase the homogeneity of newly developed code both in format and semantics,
 * ease understanding of code in the team of SINGULAR developers,
 * ease maintenance of the code, and
 * increase its reusability.

The given style guide is _not_ fixed, in the sense that it may be
updated and modified at any time, in order to further increase its utility. 
**On the other hand, developers are expected to stick to the
respective latest version of this style guide.** 
In case of minor changes to this style guide, old code sources need _not_ be adjusted to obey the newest version of the style guide. However, in case of major changes such adjustments may become advisable.

There may be collisions with other existing and widely-used style guides, e.g. when including `std` sources. Such code _may be used as it is_, i.e., it does _not_ need to be wrapped in code which conforms to the style guide at hand.

See also the [Build System description](@ref bs_page) and the [Legacy C programming style guide](@ref legacystyleguide).

## Naming Conventions

For the rest of this document, we use the term _method_ for any algorithm which is declared as part of a class definition, including static algorithms. For all other algorithmic code we use the term _procedure_. By the term _routines_, we refer to all methods and procedures.

This paragraph defines all conventions relating to names of files, classes, methods, procedures, and variables.

### General Remarks

 * **As a general rule we state that all names should be meaningful and relate to the objects they name.**
 * All names are in British English.
 * Names exclusively consist of lower and upper case letters of the english alphabet, digits, and the underscore character, `_`. (Note that this excludes all other characters, such as German umlauts.)
 * Each name complies to one of the below specified patterns. Besides these, there are _no additional valid names_. All patterns are explained by examples and an informal description. For a formal definition in the _Backus Naur Formalism_ (BNF) we point the reader to the Appendix below.

### Naming Conventions for Files

 * Examples: `bucketPoly.h, myStructure_T.c`
 * Informal description:
    * File names are in _lowerCamelCase_, followed by the dot `.` and a file extension.
    * One exception to this rule are files containing template definitions. There names end with two underscores and the capital letter `T` (followed by the dot and the file extension).
    * Header files have the extension `h`. (I.e., `hpp` is not allowed.)
    * C / C++ sources end with `c` and `cc`, respectively.
 * Other file extensions, besides `h`, `c`, and `cc` may be sensible, e.g. `l` (_el_) for grammar files, etc.
 * Special first _words_ in a file name may be used to indicate their primary purpose, e.g. `test...`. Note that thereby, such files will nicely group in alphabetically ordered file explorers.
 * If possible, files should be named after an important class or routine implemented in that file.
 * All files in the entire source code tree _must_ have mutually distinct names, in order to avoid ambiguities, e.g. when including numerous header files. Distinctness must hold _modulo capitalization_, that is, the file names `BucketPoly.h` and `bucketPoly.h` will _not_ be regarded distinct.<br />
   Note that this rule may be hard to ensure over time, as different developers may create different files with the same name(!) in different sub-folders. Thus, the rule needs to be re-established whenever a violation becomes obvious.

### Naming Conventions for Classes and Types

 * Example: `BucketPoly`
 * Informal description:
    * Class and type names are in UpperCamelCase.

### Naming Conventions for Routines

(See the above remark on the distinction between the terms _method_, _procedure_, and _routine_.)

 * Examples: `gcdAlgorithmV2, isPrime, areCoprime, getValue`
 * Informal description:
    * Routines names are in lowerCamelCase, i.e., constructed as class names, except the first letter is lower-case.
 * Private class attributes implement so-called _getter and setter methods_. Their names start with `get` and `set`, respectively.
 * For performance reasons, short routines should be declared `static inline`, although the inlining of code cannot be enforced since `inline` declarations are just recommendations to the compiler but _may be ignored_. Getter and setter methods are canonical candidates for inlining.
 * Routines with return type `bool` start with `is` or `are`.

### Naming Conventions for Variables

 * Examples: `SING_NDEBUG, nPolyCounter, i, m_pValueList`
 * Informal description:
    * Global preprocessor constants, such as `SING_NDEBUG`, consist of capitalized words separated by the underscore character `_`. Capitalization is only allowed for preprocessor constants, i.e., all other variables _must not _be capitalized.
    * Variables which are locally used in routines are in lowerCamelCase. If they are used in just one block, e.g. in a `for` loop, the _may_ consist of just one lower-case letter. In longer variable names, the first letter _may_ be used to indicate the type of the variable. Recommendations for such indications _may_ be these (Note that this is _not_ compulsory):
       * `p` for a pointer, as e.g. in `pSyzList`, but also `p` for `poly` is sensible.
       * `b` for a `bool`, as e.g. in `bConditionXHolds`,
       * `n` for anything that can be counted, i.e. `int`, or `long`,
       * `s` for a `string`,
       * `c` for a `char`,
       * `d` for a `double`,
       * `f` for a `float`,
       * `it` for an `Iterator`,
       * `a` for an `array`.
    * Private member variables in a class declaration start with `m_`, followed by lowerCamelCase; see the above example. Again, the first letter of the lowerCamelCase part may be used to indicate the type of the variable.
 * **Note that we do not allow for public class variables.** Public access must be implemented via publicly declared class methods which retrieve or set private class member variables. Often, this will be realized just by declaring getters and setters as public methods (as opposed to their normal private declaration).
 * Preprocessor constants _must not_ be used for representing variables with a mathematical or algorithm-related meaning. The proper way for representing these variables is a `const` variable declaration which will also allow for type checking at compile time.

## Structuring the Code

### General

 * The use of `using namespace foo::bar;` in header files is forbidden; due to possible ambiguities when including numerous header files.
 * Class variables of basic C type need to be initialized using a default value.
 * If a programmer does not want an instance of a class to be copied, then the **copy** and **assignment** constructors need to be declared `private`.
 * **copy, assignment** and **default** constructors of any class are _either all implemented or all not implemented_.
 * Routines that are intended to leave the object `* this` unaltered need to be declared as `const`.
 * Likewise, routine parameters that are intended to be left unaltered by that routine need to be declared as `const`.
 * The usage of `goto`, `continue` and `break` is forbidden; with the following exceptions:
    * `break` may be used in `case`, `for`, `loop` and `while` blocks to allow for an immediate termination of that block.
    * `goto` may be used in very rare cases by experts only, for reasons of clarity of the code.

### Includes in Header Files (*.h)

 * Any header file, e.g., `BucketPoly.h` _must_ start with a guard, that is, the following lines of code

~~~~~~~~~~~~~~~{.c}
    #ifndef BUCKET_POLY_H
    #define BUCKET_POLY_H
~~~~~~~~~~~~~~~

   and must end with the line

~~~~~~~~~~~~~~~{.c}
    #endif
~~~~~~~~~~~~~~~

 * The guard _should be_ the capitalized filename with the dot `.` replaced by an underscore `_` and suitably inserted additional underscores (corresponding to the camelCase structure of the filename, see example).
 * Only those header files are to be included that are actually needed inside the including piece of code.
 * Do not include `mod2.h` in header files.
 * The proper ordering of `include` statements in header files is:
    1. System related includes,
    2. additional third-party sources,
    3. SINGULAR related includes (in order to overwrite defect symbols)
 * To ease readability, any `#endif` is followed by a _new line_ containing a comment in `/*, */` which repeats the corresponding `#ifndef` information (but without the keyword `ifdef` or `ifndef`), e.g.

~~~~~~~~~~~~~~~{.c}
    #endif
    /* BUCKET_POLY_H */
~~~~~~~~~~~~~~~

   (Here we demand a _new line_ in order to avoid compiler warnings concerning characters following the `#endif` statement in the same line of code.)

### Includes in Source Files (*.c, *.cc)

The proper ordering of `include` statements in source files is:
 1. System related includes,
 2. `include mod2.h`,
 3. System related includes, which are optional (depending on `mod2.h`)
 4. all remaining and actually needed `include` statements.

### On the Use of Macros

 * The use of macros should be limited to rare cases, when other constructs will not work or would result in unclear code.
 * Macros may be used for compiler switches.
 * Moreover, whenever `inline` mechanisms are not applicable, e.g., when using the system variables `_FILE_` and `_LINE_,` macros may as well be used.

### Simple Checks and Further Debugging Utilities

 * All routines are expected to check their input parameters against their specification (see paragraph on _In-Code Documentation_), and – as far as possible – for sensibility regarding their possible combination in the debug version (i.e. if `SING_NDEBUG` is NOT defined).
 * Likewise, if easy to implement, all relations among output parameters that are to be ensured by a method, need to be checked at the end of the method.
 * The afore mentioned checks for input and output parameters of a method will only be checked in debug mode. Thus they do not influence runtime performance of any non-debug version of SINGULAR.
 * Each `case` block implements a `default` mode of operation. In case this default must never be reached, the `default` implements an error message or some similar behavior which supports debugging.

### Formatting

 * Any line of code is at most 80 characters long.
 * In case of a non-fitting routine declaration, the declaration may be linefeeded between routine parameters. Then, the next line has to be indented such that method parameters are left-aligned, as in the following example:

~~~~~~~~~~~~~~~
C& C::multiplyWith10Cs(C& c1, C& c2, ... 
                       C& c9, C& c10)
~~~~~~~~~~~~~~~

 * Parenthesizes `{` and `}` of blocks which do not fit in a single line are arranged such that they stand in the same text column, i.e., the closing `}` stand exactly below their corresponding opening `{`.
 * Each sub-block is indented by exactly 2 white-spaces with respect to its immediate parent block. The only exception to that rule are `namespaces`: the first level of indentation of code inside `namespaces` is omitted.
 * Corresponding `if` and `else` start in the same text column.
 * The usage of **Tabs** is forbidden, because different editors may provide different interpretations of them. Instead, **whitespaces** are consequently used.

### In-Code Documentation

 * In general, all documentation in the code is going to be processed by Doxygen. Therefore, documentation has to follow Doxygen syntax.<br /> 
   In order to ease documentation, template files for `*.h` and `*.cc` files, which conform to both this style guide and Doxygen syntax are provided. 
   See [C source template](@ref templates/template.c), [header template](@ref templates/template.h) and [C++ source template](@ref templates/template.cc). 
 * Each source code file starts with some version information, author, and a short comment about its purpose.
 * Each routine is preceded by a Doxygen-conformal comment about its input parameters, output values, relations that are expected to hold among them, and possible side effects on other variables, if any.
 * Difficult and more involved algorithmic parts of routines are documented directly  before or between the corresponding lines of code, even though these comments may not appear in the Doxygen-generated developer's manual.
 * Even harder algorithmic parts must come with a link to related literature which clarifies the respective passages of the algorithm.

## BNF Specification for Naming Conventions

~~~~~~~~~~~~~~~
Letter ::= A | B | ... | Z
letter ::= a | b | ... | z
letters ::= λ | letter letters
Underscore ::= _
Digit ::= 0 | 1 | ... | 9
Digits ::= λ | Digit Digits
Word ::= Letter letters Digits
word ::= letter letters Digits
CapitalizedWord ::= Letter | Letter CapitalizedWord
UpperCamelCase ::= Word | Word UpperCamelCase
lowerCamelCase ::= word | word UpperCamelCase
Dot ::= .
Suffix ::= h | c | cc | {...further}
Filename ::= lowerCamelCase Dot Suffix
ClassOrTypeName ::= UpperCamelCase
Methodname ::= lowerCamelCase
LocalBlockVariable ::= letter
PreprocessorConstant ::= CapitalizedWord | CapitalizedWord _ PreprocessorConstant
LocalVariable ::= lowerCamelCase
MemberVariable ::= m Underscore lowerCamelCase
~~~~~~~~~~~~~~~

