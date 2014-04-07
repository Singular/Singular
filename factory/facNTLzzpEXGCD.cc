// -*- c++ -*-
//*****************************************************************************
/** @file facNTLzzpEXGCD.cc
 *
 * @author Martin Lee
 *
 * @note the following code is slightly modified code out of
 * lzz_pEX.c from Victor Shoup's NTL. Below is NTL's copyright notice.
 *
 * Univariate GCD and extended GCD over Z/p[t]/(f)[x] for reducible f
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
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



#include "config.h"


#include "facNTLzzpEXGCD.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_NTL

long InvModStatus (zz_pE& x, const zz_pE& a)
{
   return InvModStatus(x._zz_pE__rep, a._zz_pE__rep, zz_pE::modulus());
}

static
void SetSize(vec_zz_pX& x, long n, long m)
{
   x.SetLength(n);
   long i;
   for (i = 0; i < n; i++)
      x[i].rep.SetMaxLength(m);
}

void tryPlainRem(zz_pEX& r, const zz_pEX& a, const zz_pEX& b, vec_zz_pX& x,
                 bool& fail)
{
   long da, db, dq, i, j, LCIsOne;
   const zz_pE *bp;
   zz_pX *xp;


   zz_pE LCInv, t;
   zz_pX s;

   da = deg(a);
   db = deg(b);

   if (db < 0) Error("zz_pEX: division by zero");

   if (da < db) {
      r = a;
      return;
   }

   bp = b.rep.elts();

   if (IsOne(bp[db]))
      LCIsOne = 1;
   else {
      LCIsOne = 0;
      fail= InvModStatus (LCInv, bp[db]);
      //inv(LCInv, bp[db]);
      if (fail)
        return;
   }

   for (i = 0; i <= da; i++)
      x[i] = rep(a.rep[i]);

   xp = x.elts();

   dq = da - db;

   for (i = dq; i >= 0; i--) {
      conv(t, xp[i+db]);
      if (!LCIsOne)
	 mul(t, t, LCInv);
      NTL::negate(t, t);

      for (j = db-1; j >= 0; j--) {
	 mul(s, rep(t), rep(bp[j]));
	 add(xp[i+j], xp[i+j], s);
      }
   }

   r.rep.SetLength(db);
   for (i = 0; i < db; i++)
      conv(r.rep[i], xp[i]);
   r.normalize();
}

void tryPlainDivRem(zz_pEX& q, zz_pEX& r, const zz_pEX& a, const zz_pEX& b,
                    bool& fail)
{
   long da, db, dq, i, j, LCIsOne;
   const zz_pE *bp;
   zz_pE *qp;
   zz_pX *xp;


   zz_pE LCInv, t;
   zz_pX s;

   da = deg(a);
   db = deg(b);

   if (db < 0) Error("zz_pEX: division by zero");

   if (da < db) {
      r = a;
      clear(q);
      return;
   }

   zz_pEX lb;

   if (&q == &b) {
      lb = b;
      bp = lb.rep.elts();
   }
   else
      bp = b.rep.elts();

   if (IsOne(bp[db]))
      LCIsOne = 1;
   else {
      LCIsOne = 0;
      fail= InvModStatus (LCInv, bp[db]);
      //inv(LCInv, bp[db]);
      if (fail)
        return;
   }

   vec_zz_pX x;

   SetSize(x, da+1, 2*zz_pE::degree());

   for (i = 0; i <= da; i++)
      x[i] = rep(a.rep[i]);

   xp = x.elts();

   dq = da - db;
   q.rep.SetLength(dq+1);
   qp = q.rep.elts();

   for (i = dq; i >= 0; i--) {
      conv(t, xp[i+db]);
      if (!LCIsOne)
	 mul(t, t, LCInv);
      qp[i] = t;
      NTL::negate(t, t);

      for (j = db-1; j >= 0; j--) {
	 mul(s, rep(t), rep(bp[j]));
	 add(xp[i+j], xp[i+j], s);
      }
   }

   r.rep.SetLength(db);
   for (i = 0; i < db; i++)
      conv(r.rep[i], xp[i]);
   r.normalize();
}


void tryNTLGCD(zz_pEX& x, const zz_pEX& a, const zz_pEX& b, bool& fail)
{
   zz_pE t;

   if (IsZero(b))
      x = a;
   else if (IsZero(a))
      x = b;
   else {
      long n = max(deg(a),deg(b)) + 1;
      zz_pEX u(INIT_SIZE, n), v(INIT_SIZE, n);

      vec_zz_pX tmp;
      SetSize(tmp, n, 2*zz_pE::degree());

      u = a;
      v = b;
      do {
         tryPlainRem(u, u, v, tmp, fail);
         if (fail)
           return;
         swap(u, v);
      } while (!IsZero(v));

      x = u;
   }

   if (IsZero(x)) return;
   if (IsOne(LeadCoeff(x))) return;

   /* make gcd monic */


   fail= InvModStatus (t, LeadCoeff(x));
   if (fail)
     return;
   mul(x, x, t);
}

void tryNTLXGCD(zz_pEX& d, zz_pEX& s, zz_pEX& t, const zz_pEX& a,
                const zz_pEX& b, bool& fail)
{
   zz_pE z;


   if (IsZero(b)) {
      set(s);
      clear(t);
      d = a;
   }
   else if (IsZero(a)) {
      clear(s);
      set(t);
      d = b;
   }
   else {
      long e = max(deg(a), deg(b)) + 1;

      zz_pEX temp(INIT_SIZE, e), u(INIT_SIZE, e), v(INIT_SIZE, e),
            u0(INIT_SIZE, e), v0(INIT_SIZE, e),
            u1(INIT_SIZE, e), v1(INIT_SIZE, e),
            u2(INIT_SIZE, e), v2(INIT_SIZE, e), q(INIT_SIZE, e);


      set(u1); clear(v1);
      clear(u2); set(v2);
      u = a; v = b;

      do {
         fail= InvModStatus (z, LeadCoeff(v));
         if (fail)
           return;
         DivRem(q, u, u, v);
         swap(u, v);
         u0 = u2;
         v0 = v2;
         mul(temp, q, u2);
         sub(u2, u1, temp);
         mul(temp, q, v2);
         sub(v2, v1, temp);
         u1 = u0;
         v1 = v0;
      } while (!IsZero(v));

      d = u;
      s = u1;
      t = v1;
   }

   if (IsZero(d)) return;
   if (IsOne(LeadCoeff(d))) return;

   /* make gcd monic */

   fail= InvModStatus (z, LeadCoeff(d));
   if (fail)
     return;
   mul(d, d, z);
   mul(s, s, z);
   mul(t, t, z);
}
#endif

