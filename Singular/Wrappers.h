#ifndef WRAPPERS_H
#define WRAPPERS_H

#ifdef HAVE_WRAPPERS

#include "ring.h"
#include "polys.h"
#include "PrettyPrinter.h"

/*! \mainpage C++ wrappers for SINGULAR and related systems
 *
 *  \section design Design Overview
 *  The following picture shows a UML-like design of the classes implemented so far,
 *  and their relationship.
 *  \image html "file:///C:/Work/C++Code/inside SINGULAR/C++Wrappers/Overview1.jpg"
 *
 *  \section howto How to run this code inside SINGULAR
 *  The following steps need to be done to run the C++ wrapper code inside SINGULAR:<br><br>
 *  Re-build and run SINGULAR:<br>
 *  - checkout the latest code and compile SINGULAR,<br>
 *  - go to <c>/SINGULAR</c> and perform <c>make clean</c>,<br>
 *  - go to <c>/kernel</c> and perform <c>make clean</c>,<br>
 *  - open <c>/SINGULAR/mod2.h</c> and include the line <c>#define HAVE_WRAPPERS 1</c>,<br>
 *  - open <c>/kernel/mod2.h</c> and include the line <c>#define HAVE_WRAPPERS 1</c>,<br>
 *  - go to <c>/SINGULAR</c> and perform <c>make</c>,<br>
 *  - go to <c>/kernel</c> and perform <c>make</c> (Now you have a runnable SINGULAR version
 *    including the C++ wrapper code.),<br>
 *  - go to <c>/SINGULAR</c> and perform <c>./Singular</c> to run the newly built local
 *    executable<br>
 *
 *  Call the new code:<br>
 *  - you may first declare a ring, e.g. by typing <c>ring r;</c>,<br>
 *  - type <c>system("c++wrappers", 0);</c> to perform <c>testWrappers</c> (see file Wrappers.h) without
 *    detailed printout to the console,<br>
 *  - type <c>system("c++wrappers", 1);</c> to perform <c>testWrappers</c> (see file Wrappers.h) with
 *    detailed printout to the console,<br>
 *  - declare a ring and two polynomials f and g; then type <c>system("c++wrappers", f, g);</c>
 *    to perform <c>wrapSINGULARPolys</c> (see file Wrappers.h).
 */

/*! type definition for the type of the instance counter inside the class ReferenceCounter */
typedef unsigned long ReferenceCounterType;

/*! type definition for SINGULAR-internal polynomials */
typedef poly SingularPoly;

/*! type definition for SINGULAR-internal rings */
typedef ring SingularRing;

/*!
 *  A method for performing a series of tests with the wrapper code.<br>
 *  More concretely, the following tests will be performed:<br>
 *  - Test0: Creation of RingWrapper from current SINGULAR ring, if any,<br>
 *  - Test1: Creation and destruction of instances of RingWrapper,<br>
 *  - Test2: Creation, copying, assignment, and destruction of instances of PolyWrapper,<br>
 *  - Test3: Addition of compatible instances of PolyWrapper,<br>
 *  - Test4: Addition of incompatible instances of PolyWrapper.<br>
 *
 *  After compiling SINGULAR and including the line<br>
 *  <c>#define HAVE_WRAPPERS 1</c><br>
 *  in <c>/SINGULAR/mod2.h</c> and in <c>/kernel/mod2.h</c>, the user may call this
 *  method by typing <c>system("c++wrappers", 0)</c> (without detailed console printout),
 *  or <c>system("c++wrappers", 1)</c> (with detailed console printout).
 *  @param detailedOutput if true this enforces a very detailed console output including internal method calls etc.
 */
void testWrappers (bool detailedOutput);

/*!
 *  A method for wrapping SINGULAR-internal polynomials as instances of PolyWrapper
 *  and afterwards computing their sum as an instance of PolyWrapper.<br>
 *  After compiling SINGULAR and including the line<br>
 *  <c>#define HAVE_WRAPPERS 1</c><br>
 *  in <c>/SINGULAR/mod2.h</c> and in <c>/kernel/mod2.h</c>, the user may call this
 *  method by first declaring two polys f and g, and typing
 *  <c>system("c++wrappers", f, g)</c>.
 *  @param sp1 a SINGULAR-internal poly
 *  @param sp2 a SINGULAR-internal poly
 */
void wrapSINGULARPolys (const SingularPoly& sp1, const SingularPoly& sp2);

/*! PrettyPrinter used throughout all wrapper code
    for pretty-printing detailed output to the console
    if requested */
extern PrettyPrinter prpr;

#endif
/* HAVE_WRAPPERS */

#endif
/* WRAPPERS_H */
