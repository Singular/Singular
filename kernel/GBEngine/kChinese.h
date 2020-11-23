#ifndef KCHINESE_H
#include "misc/auxiliary.h"

#include "misc/intvec.h"
#include "polys/monomials/p_polys.h"
#include "polys/matpol.h"
#include "polys/simpleideals.h"

#include <gmp.h>
/*2
* xx,q: arrays of length 0..rl-1
* xx[i]: SB mod q[i]
* assume: char=0
* assume: q[i]!=0
* destroys xx
*/
poly p_ChineseRemainder(poly *xx, mpz_ptr *x,mpz_ptr *q, int rl, mpz_ptr *C ,const ring R);
ideal id_ChineseRemainder_0(ideal *xx, number *q, int rl, const ring r);
ideal id_Farey_0(ideal xx, number N, const ring r);
#endif
