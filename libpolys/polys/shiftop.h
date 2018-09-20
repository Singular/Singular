#include "misc/auxiliary.h"
#include "monomials/ring.h"
#include "../kernel/GBEngine/shiftgb.h"

poly shift_pp_Mult_mm(poly p, const poly m, const ring r);
poly shift_p_Mult_mm(poly p, const poly m, const ring r);
poly shift_pp_mm_Mult(poly p, const poly m, const ring r);
poly shift_p_mm_Mult(poly p, const poly m, const ring r);
poly shift_p_Minus_mm_Mult_qq(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring r);
poly shift_pp_Mult_mm_Noether_STUB(poly p, const poly m, const poly spNoether, int &ll, const ring ri);
poly shift_pp_Mult_Coeff_mm_DivSelectMult_STUB(poly p,const poly m, const poly a, const poly b, int &shorter,const ring r);
poly shift_pp_Mult_Coeff_mm_DivSelect_STUB(poly p, const poly m, int &shorter, const ring r);
void p_mLPUnShift(poly m, const ring ri);
void p_LPUnShift(poly p, const ring ri);
void p_LPExpVappend(int *m1ExpV, int *m2ExpV, int m1Length, int m2Length, const ring ri);
void p_LPExpVprepend(int *m1ExpV, int *m2ExpV, int m1Length, int m2Length, const ring ri);
void WriteLPExpV(int *expV, ring ri);
