
#ifndef NTL_config__H
#define NTL_config__H

/*************************************************************************

                          NTL Configuration File
                          ----------------------

This file may be modified prior to building NTL so as to specify
some basic configuration options, and to customize
how code is generated so as to improve performance.

The Basic Configuration Options must be set by hand.  If you use the
configuration wizard, then these flags should be set before
the installation process begins;  there values will be retained
by the wizard.

The Performance Options can be set either by hand, by editing this 
file, or (on most Unix platforms) can be set automatically using
the configuration wizard which runs when NTL is installed.

All NTL include (".h") files include this file.
By setting these flags here, instead of on the compiler command line,
it is easier to guarantee that NTL library and client code use
consistent settings.  


                                How to do it
                                ------------

You override NTL's default code generation strategies by setting
various flags, as described below.  To set a flag, just replace the
pre-processor directive 'if 0' by 'if 1' for that flag, 
which causes the appropriate macro to be defined.  Of course, 
to unset a flag, just replace the 'if 1' by an 'if 0'.

 *************************************************************************/



/*************************************************************************
 *
 * Basic Configuration Options
 *
 *************************************************************************/


 /* None of these flags are set by the configuration wizard;
  * they must be set by hand, before installation begins.
  */

#if 0
#define NTL_STD_CXX

/* 
 * Use this flag if you want to use the "Standard C++" version of NTL.
 * In this version, all of NTL is "wrapped" inside the namespace NTL,
 * and are no longer directly accessible---you must either use
 * explicit qualification, or using directives, or using declarations.
 * However, note that all names that begin with "NTL_" are macros,
 * and as such do not belong to any namespace.
 * Additionally, instead of including the standard headers
 * <stdlib.h>, <math.h>, and <iostream.h>, the standard headers
 * <cstdlib>, <cmath>, and <iostream> are included.
 * These "wrap" some (but not all) names in namespace std.
 * Also, the 'nothrow' version on the 'new' operator is used.
 *
 * To re-build after changing this flag: rm *.o; make ntl.a
 */

#endif

/* The following three flags may be used if you want to use some
 * of the features of Standard C++, but your compiler is deficient.
 * Instead of setting the NTL_STD_CXX, you can set any subset 
 * of the these three.  Setting all three of these flags is equivalent
 * to setting NTL_STD_CXX.  No harm is done if NTL_STD_CXX is set
 * and some of the following three flags are set.
 *
 * To re-build after changing any of these flags: rm *.o; make ntl.a
 */

#if 0
#define NTL_PSTD_NNS

/* Set if NTL library components are to be wrapped in namespace 'NTL'. */

#endif

#if 0
#define NTL_PSTD_NHF

/* Set if you want to use the new header files <cstdlib>, <cmath>, and 
 * <iostream>, instead of the traditional header files <stdlib.h>,
 * <math.h>, and <iostream.h>.
 * If new header files are used, then it is assumed that all standard 
 * library components are wrapped in namespace std; otherwise,
 * it is assumed that all standard library components are in the
 * global namespace.
 *
 * Also, when set, some internal NTL files use the header <fstream>
 * in place of <fstream.h>.                                                      
 */

#endif

#if 0
#define NTL_PSTD_NTN

/* Set if you want to use the 'nothrow' version of new. */

#endif


#if 0
#define NTL_GMP_LIP

/* 
 * Use this flag if you want to use GMP as the long integer package.
 * This can result in significantly faster code on some platforms.
 * It requires that the GMP package (version >= 3.1) has already been
 * installed.  You will also have to set the variables GMP_INCDIR,
 * GMP_LIBDIR, and GMP_LIB in the makefile (these are set automatically
 * by the confiuration script when you pass the flag NTL_GMP_LIP=on
 * to that script.
 *
 * Beware that setting this flag can break some older NTL codes.
 * If you want complete backward compatability, but not quite
 * the full performance of GMP, use the flag NTL_GMP_HACK below.
 * See the full NTL documentation for more details.
 *
 * To re-build after changing this flag:
 *   rm *.o; make setup3; make ntl.a
 * You may also have to edit the makefile to modify the variables
 * GMP_INCDIR, GMP_LIBDIR, and GMP_LIB.
 */

#elif 0
#define NTL_GMP_HACK

/* 
 * Use this flag if you want to use GMP as the long integer package.
 * This can result in significantly faster code on some platforms.
 * It requires that the GMP package (version >= 2.0.2) has already been
 * installed.  You will also have to set the variables GMP_INCDIR,
 * GMP_LIBDIR, and GMP_LIB in the makefile (these are set automatically
 * by the confiuration script when you pass the flag NTL_GMP_HACK=on
 * to that script.
 *
 * Unlike the NTL_GMP_LIP flag above, setting this flag maintains
 * complete backward compatability with older NTL codes, but
 * you do not get the full performance of GMP.
 *
 * To re-build after changing this flag:
 *   rm lip.o; make setup3; make ntl.a
 * You may also have to edit the makefile to modify the variables
 * GMP_INCDIR, GMP_LIBDIR, and GMP_LIB.
 *
 */

#endif


#if 0
#define NTL_LONG_LONG_TYPE long long

/*
 *   If you set NTL_LONG_LONG, you may need to override the default
 *   name of this "nonstandard" type.  For example, under MS C++,
 *   the right name is __int64.
 *   
 *   This flag is irrelevant when NTL_GMP_LIP is set.
 *
 *   To re-build after changing this flag: rm lip.o; make ntl.a
 */

#endif


#if 0
#define NTL_CXX_ONLY

/*
 *   It is possible to compile everything using C++ only.
 *   If you want to do this, make CC and CXX in the makefile the same.
 *   You may also want to set this flag, which eliminates some
 *   "C" linkage that is no longer necessary.
 *   However, it should still work without it.
 *   
 *   This flag can be set independently of NTL_STD_CXX.
 *   All functions that may have "C" linkage are never wrapped in
 *   namespace NTL;  instead, their names always start with "_ntl_",
 *   and as such, they should not conflict with other global names.
 *   All such names are undocumented, and should never be used 
 *   by NTL clients under normal circumstances.
 *
 *   To re-build after changing this flag: rm *.o; make ntl.a
 */

#endif

 
#if 0
#define NTL_RANGE_CHECK

/*
 *   This will generate vector subscript range-check code.
 *   Useful for debugging, but it slows things down of course.
 *
 *   To re-build after changing this flag: rm *.o; make ntl.a
 */

#endif





#if 0
#define NTL_NO_INIT_TRANS

/*
 *   Without this flag, NTL uses a special code sequence to avoid
 *   copying large objects in return statements.  However, if your
 *   compiler optimizes away the return of a *named* local object,
 *   this is not necessary, and setting this flag will result
 *   in *slightly* more compact and efficient code.  Although
 *   the emeriging C++ standard allows compilers to perform
 *   this optimization, I know of none that currently do.
 *   Most will avoid copying *temporary* objects in return statements,
 *   and NTL's default code sequence exploits this fact.
 *
 *   To re-build after changing this flag: rm *.o; make ntl.a
 */

#endif


#if 0
#define NTL_X86_FIX

/*
 *  Forces the "x86 floating point fix", overriding the default behavior.
 *  By default, NTL will apply the "fix" if it looks like it is
 *  necessary, and if knows how to fix it.
 *  The problem addressed here is that x86 processors sometimes
 *  run in a mode where FP registers have more precision than doubles.
 *  This will cause code in quad_float.c some trouble.
 *  NTL can normally correctly detect the problem, and fix it,
 *  so you shouldn't need to worry about this or the next flag.

 *  To re-build after changing this flag: rm quad_float.o; make ntl.a
 *  
 */

#elif 0
#define NTL_NO_X86_FIX
/*
 *  Forces no "x86 floating point fix", overriding the default behavior.

 *  To re-build after changing this flag: rm quad_float.o; make ntl.a
 */

#endif




/*************************************************************************
 *
 *  Performance Options
 *
 *************************************************************************/


/* One can choose one of four different stragtegies for long integer
 * arithmetic: the default, NTL_LONG_LONG, NTL_AVOID_FLOAT, or NTL_SINGLE_MUL.
 * The configuration wizard will choose among the first three; the use of
 * NTL_SINGLE_MUL is not generally recommended.
 *   
 * These flags are irrelevant when NTL_GMP_LIP is set, and are simply ignored,
 * except for NTL_SINGLE_MUL -- setting that causes a complie-time error.
 * 
 */

#if 0
#define NTL_LONG_LONG

/*
 *   RECOMMENDED FOR some x86  PLATFORMS
 *
 *   For platforms that support it, this flag can be set to cause
 *   the low-level multiplication code to use the type "long long",
 *   which on some platforms yields a significant performance gain,
 *   but on others, it can yield no improvement and can even
 *   slow things down.
 *
 *   The only platform where I know this helps is Linux/Pentium,
 *   but even here, the gcc compiler is less than impressive with 
 *   it code generation.
 *
 *   See below (NTL_LONG_LONG_TYPE) for how to use a type name 
 *   other than "long long".
 *
 *   If you set NTL_LONG_LONG, you might also want to set
 *   the flag NTL_TBL_REM (see below).
 *
 *   To re-build after changing this flag:  rm lip.o; make ntl.a
 */

#elif 0
#define NTL_AVOID_FLOAT

/*
 *   RECOMMENDED FOR AIX/PowerPC and some x86 PLATFORMS
 *
 *   On machines with slow floating point or---more comminly---slow int/float
 *   conversions, this flag can lead to faster code.
 *
 *   I get much better code on the AIX/PowerPC platform than with the
 *   default setting or with NTL_LONG_LONG flag.  
 *
 *   I also get slightly better code on the Linux/Pentium-II platform 
 *   with this flag than with the NTL_LONG_LONG flag; 
 *   however, on a Pentium-I, NTL_LONG_LONG is much better.
 *
 *   If you set NTL_AVOID_FLOAT, you should probably also
 *   set NTL_TBL_REM (see below).
 *
 *   To re-build after changing this flag:  rm lip.o; make ntl.a
 */

#elif 0
#define NTL_SINGLE_MUL 

/*   This was developed originally to improve performance on
 *   ancient Sparc stations that did not have built-in integer mul
 *   instructions.  Unless you have such an old-timer, I would not
 *   recommend using this option.  This option only works on
 *   32-bit machines with IEEE floating point, and is not truly
 *   portable.  If you use this option, you get a 26-bit radix.
 *
 *   To re-build after changing this flag: rm *.o; make ntl.a
 */

#endif



/* The next three flags, NTL_TBL_REM, NTL_AVOID_BRANCHING, NTL_FFT_PIPELINE,
 * are also set by the configuration wizard.  
 */



#if 0
#define NTL_TBL_REM

/*
 *   RECOMMENDED FOR AIX/PowerPC and some x86 PLATFORMS
 *
 *   With this flag, some divisions are avoided in the
 *   ZZ_pX multiplication routines.  If you use the NTL_AVOID_FLOAT 
 *   or NTL_LONG_LONG flags, then you should probably use this one too.
 *
 *   Irrelevent when NTL_GMP_LIP is set.
 *
 *   To re-build after changing this flag: 
 *      rm lip.o; make ntl.a
 */

#endif


#if 0
#define NTL_AVOID_BRANCHING

/*
 *   With this option, branches are replaced at several 
 *   key points with equivalent code using shifts and masks.
 *   Recommended for use with RISC architectures, especially
 *   ones with deep pipelines and high branch penalities.
 *   This flag is becoming less helpful as newer machines
 *   have much smaller branch penalties, but still may be worth a try.
 *
 *   To re-build after changing this flag: rm *.o; make ntl.a
 */

#endif


#if 0
#define NTL_FFT_PIPELINE

/*
 *   If using NTL_AVOID_BRANCHING, you might want to try this as well.
 *   This causes the FFT routine to use a software pipeline.
 *
 *   To re-build after changing this flag: rm FFT.o; make ntl.a
 */

#endif


/* The following flag is not set by the configuration wizard;  its use
 * is not generally recommended.
 */

 
#if 0
#define NTL_FAST_INT_MUL

/*
 *   Really esoteric.
 *   If using NTL_SINGLE_MUL, and your machine
 *   has a fast integer multiply instruction, this might yield
 *   faster code.  Experiment!
 *
 *   Irrelevent when NTL_GMP_LIP is set.
 *
 *   To re-build after changing this flag: rm *.o; make ntl.a
 */

#endif







#endif
