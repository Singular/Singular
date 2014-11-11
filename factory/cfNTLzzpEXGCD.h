/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfNTLzzpEXGCD.h
 *
 * This file defines functions for univariate GCD and extended GCD
 * over Z/p[t]/(f)[x] for reducible f
 *
 * @note the following code is slightly modified code out of
 * lzz_pEX.h from Victor Shoup's NTL. Below is NTL's copyright notice.
 *
 * ABSTRACT: Langemyr, McCallum "The Computation of Polynomial Greatest Common
 * Divisors over an algebraic number fields"
 *
 * @author Martin Lee
 *


                  COPYRIGHT NOTICE
		    for NTL 5.5
	  (modified for Singular 2-0-6 - 3-1)

NTL -- A Library for Doing Number Theory
Copyright (C) 1996-2009  Victor Shoup

The most recent version of NTL is available at http://www.shoup.net

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

This entire copyright notice should be placed in an appropriately
conspicuous place accompanying all distributions of software that
make use of NTL.

The above terms apply to all of the software modules distributed with NTL,
i.e., all source files in either the ntl-xxx.tar.gz or WinNTL-xxx.zip
distributions.  In general, the individual files do not contain
copyright notices.

Note that the quad_float package is derived from the doubledouble package,
originally developed by Keith Briggs, and also licensed unger the GNU GPL.
The files quad_float.c and quad_float.h contain more detailed copyright
notices.

Note that the traditional long integer package used by NTL, lip.c, is derived
from---and represents an extensive modification of---
a package originally developed and copyrighted by Arjen Lenstra,
who has agreed to renounce any copyright claims on the particular
version of the long integer package appearing in NTL, so that the
this package now is covered by the GNU GPL as well.

Note that the alternative long integer package used by NTL is GMP,
which is written by Torbjorn Granlund <tege@swox.com>.
GMP is licensed under the terms of the GNU Lesser General Public License.

Note that NTL makes use of the RSA Data Security, Inc. MD5 Message
Digest Algorithm.

Note that prior to version 4.0, NTL was distributed under the following terms:
   NTL is freely available for research and educational purposes.
   I don't want to attach any legalistic licensing restrictions on
   users of NTL.
   However, NTL should not be linked in a commercial program
   (although using data in a commercial
   product produced by a program that used NTL is fine).

The hope is that the GNU GPL is actually less restrictive than these
older terms;  however, in any circumstances such that GNU GPL is more
restrictive, then the following rule is in force:
versions prior to 4.0 may continue to be used under the old terms,
but users of versions 4.0 or later should adhere to the terms of the GNU GPL.
**/


#ifndef CF_NTL_ZZ_PEX_GCD_H
#define CF_NTL_ZZ_PEX_GCD_H

// #include "config.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_NTL
/// compute the GCD x of a and b, fail is set to true if a zero divisor is
/// encountered
void tryNTLGCD(zz_pEX& x,      ///<[in,out] GCD of a and b
               const zz_pEX& a,///<[in]     s.a.
               const zz_pEX& b,///<[in]     s.a.
               bool& fail      ///<[in,out] s.a.
              );

/// compute the extended GCD d=s*a+t*b, fail is set to true if a zero divisor is
/// encountered
void tryNTLXGCD(zz_pEX& d,      ///<[in,out] GCD of a and b
                zz_pEX& s,      ///<[in,out] s. a.
                zz_pEX& t,      ///<[in,out] s. a.
                const zz_pEX& a,///<[in]     s. a.
                const zz_pEX& b,///<[in]     s. a.
                bool& fail      ///<[in,out] s. a.
               );
#endif

#endif
