#include "misc/auxiliary.h"
#include "monomials/ring.h"
#include "../kernel/GBEngine/shiftgb.h"

poly shift_pp_Mult_mm(poly p, const poly m, const ring r);
poly shift_p_Mult_mm(poly p, const poly m, const ring r);
poly shift_pp_mm_Mult(poly p, const poly m, const ring r);
poly shift_p_mm_Mult(poly p, const poly m, const ring r);
poly shift_p_Minus_mm_Mult_qq(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring r);
