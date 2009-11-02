/* slightly changed for use by omalloc.h */
/*
 * defines to get the return-address for non-dmalloc_lp malloc calls.
 *
 * Copyright 2000 by Gray Watson
 *
 * This file is part of the dmalloc package.
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies, and that the name of Gray Watson not be used in advertising
 * or publicity pertaining to distribution of the document or software
 * without specific, written prior permission.
 *
 * Gray Watson makes no representations about the suitability of the
 * software described herein for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The author may be contacted via http://dmalloc.com/
 *
 * $Id$
 */

/*
 * This file contains the definition of the SET_RET_ADDR macro which
 * is designed to contain the archecture/compiler specific hacks to
 * determine the return-address from inside the malloc library.  With
 * this information, the library can display caller information from
 * calls that do not use the malloc_lp functions.
 *
 * Most of the archectures here have been contributed by other
 * individuals and may need to be toggled a bit to remove local
 * configuration differences.
 *
 * PLEASE send all submissions, comments, problems to the author.
 *
 * NOTE: examining the assembly code for x =
 * __builtin_return_address(0); with gcc version 2+ should give you a
 * good start on building a hack for your box.
 */

#ifndef __OM_RETURN_H__
#define __OM_RETURN_H__

/*************************************/

/* for Sun SparcStations with GCC */
#if __sparc && __GNUC__ > 1

/*
 * NOTE: %i7 seems to be more reliable than the [%fp+4] used by
 * __builtin_return_address.  [%fp+4] is on the stack however, meaning
 * it may be better -- less prone to be erased.  However, it produces
 * some bogus data -- it seems to return the last return-address or
 * something like that.
 *
 * Alexandre Oliva <oliva@dcc.unicamp.br> recently advised to change
 * the "=g" to a "=m".  If you are having problems, you may want to
 * return to the =g to see if it works.
 */
#define GET_RET_ADDR(file)	asm("st %%i7,%0" : \
				    "=m" (file) : \
				    /* no inputs */ )

#if 0

/* this was the default however =g was recommended to be changed to =m */
#define GET_RET_ADDR(file)	asm("st %%i7,%0" : \
				    "=g" (file) : \
				    /* no inputs */ )

/*
 * This was what the gcc __builtin_return_address returned.  The above
 * versions worked better.  It is here for reference purposes only.
 */
#define GET_RET_ADDR(file)	asm("ld [%%fp+4],%%o0; st %%o0,%0" : \
					"=g" (file) : \
					/* no inputs */ : \
					"o0")
#endif

#endif /* __sparc */

/*************************************/

/* for i[34]86 machines with GCC */
#if __i386 && __GNUC__ > 1

#define GET_RET_ADDR(file)	asm("movl 4(%%ebp),%%eax ; movl %%eax,%0" : \
				    "=g" (file) : \
				    /* no inputs */ : \
				    "eax")

#endif /* __i386 */

/*************************************/

/*
 * For DEC Mips machines running Ultrix
 */
#if __mips

/*
 * I have no idea how to get inline assembly with the default cc.
 * Anyone know how?
 */

#if 0

/*
 * NOTE: we assume here that file is global.
 *
 * $31 is the frame pointer.  $2 looks to be the return address but maybe
 * not consistently.
 */
#define GET_RET_ADDR(file)	asm("sw $2, file")

#endif

#endif /* __mips */

/******************************* contributions *******************************/

/*
 * For DEC Alphas running OSF.  from Dave Hill <ddhill@zk3.dec.com>
 * and Alexandre Oliva <oliva@dcc.unicamp.br>.  Thanks guys.
 */
#if __alpha

#ifdef __GNUC__

#define GET_RET_ADDR(file)	asm("bis $26, $26, %0" : "=r" (file))

#else /* __GNUC__ */

#include <c_asm.h>

#define GET_RET_ADDR(file)	file = (char *)asm("bis %ra,%ra,%v0")

#endif /* __GNUC__ */

#endif /* __alpha */

/*************************************/

/*
 * For Data General workstations running DG/UX 5.4R3.00 from Joerg
 * Wunsch <joerg_wunsch@julia.tcd-dresden.de>.
 */
#ifdef __m88k__

/*
 * I have no ideas about the syntax of Motorola SVR[34] assemblers.
 * Also, there may be occasions where gcc does not set up a stack
 * frame for some function, so the returned value should be taken with
 * a grain of salt. For the ``average'' function calls it proved to be
 * correct anyway -- jw
 */
#if !__DGUX__ || _DGUXCOFF_TARGET
# define M88K_RET_ADDR	"ld %0,r30,4"
#else  /* __DGUX__ && !_DGUXCOFF_TARGET: DG/UX ELF with version 3 assembler */
# define M88K_RET_ADDR	"ld %0,#r30,4"
#endif

#define GET_RET_ADDR(file)	asm(M88K_RET_ADDR : \
				    "=r" (file) : \
				    /* no inputs */)

#endif /* m88k */

/*************************************/

/*
 * SGI compilers implement a C level method of accessing the return
 * address by simply referencing the __return_address symbol. -- James
 * Bonfield <jkb@mrc-lmb.cam.ac.uk>
 */
#if defined(__sgi)

#define GET_RET_ADDR(file)	file = (void *)__return_address

#endif /* __sgi */

/*************************************/

/*
 * Stratus FTX system (UNIX_System_V 4.0 FTX release 2.3.1.1 XA/R
 * Model 310 Intel i860XP processor)
 *
 * I ended up with compiling according to full-ANSI rules (using the
 * -Xa compiler option).  This I could only do after modifying the
 * "dmalloc.h.3" in such a way that the malloc/calloc/realloc/free
 * definitions would no longer cause the compiler to bark with
 * 'identifier redeclared' (I just put an #ifdef _STDLIB_H ... #endif
 * around those functions).  -- Wim_van_Duuren@stratus.com
 */
#if defined(_FTX) && defined(i860)

/*
 * we first have the define the little assembly code function
 */
asm void ASM_GET_RET_ADDR(file)
{
%       reg     file;
        mov     %r1, file
%       mem     file;
        st.l    %r31,-40(%fp)
        orh     file@ha, %r0, %r31
        st.l    %r1, file@l(%r31)
        ld.l    -40(%fp),%r31
%       error
}
#define GET_RET_ADDR(file)	ASM_GET_RET_ADDR(file)

#endif /* _FTX & i860 */

/*************************************/

/*
 * For HP-UX PA-RISC with gcc/g++ from Jack Perdue
 * <j-perdue@tamu.edu>.
 */
#if __hpux && __hppa && __GNUC__ > 1

/*
 * This is my very first line of PA-RISC assembly ever.  I have no
 * idea if is correct, but it seems to work.  Register 2 (%r2)
 * contains the return address.  The code generated by x =
 * __builtin_return_address() was considerably longer and without a
 * PA-RISC Instruction Set guide available (not even on the web), I
 * just put togther little tidbits of knowledge I found while looking
 * for one to take a stab with this.  Like I said, it seems to work on
 * my g++ code under HPUX11 on a HP V-class system.  YMMV. -- Jack
 * Perdue (j-perdue@tamu.edu)
 */
#define GET_RET_ADDR(file)  asm("stw %%r2, %0" : "=m" (file) :  );

#endif

/*************************************/

/*
 * For Powerpc 603 based system running LynxOS 2.3.1 using gcc/gas.
 * From Shawn Carey <smc@questra.com>.
 */
#if defined(__powerpc__) && defined(__GNUC__) && !defined(__OPTIMIZE__)

/*
 * This won't compile if "-O2" is used, but it seems to work fine with
 * "-O0".  I'm no assembler expert; I was happy enough to come up with
 * something that works at all...  :-)
 */

#define GET_RET_ADDR(file) \
do { \
  asm("mflr 0"); \
  asm("stw 0,%0" : "=g" (file)); \
} while(0)

#endif /* __powerpc__ && __GNUC__ && !__OPTIMIZE__ */

/*************************************/

/********************************** default **********************************/

/* for all others, do nothing */
#ifndef GET_RET_ADDR

#if __GNUC__ > 1
#define GET_RET_ADDR(file) (file = __builtin_return_address(0))
#else
#define GET_RET_ADDR(file)	(file = 0)
#endif
#endif /* __GNUC__ > 1 */

#endif /* ! __OM_RETURN_H__ */
