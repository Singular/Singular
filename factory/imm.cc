#ifdef __MWERKS__

#include "imm.h"
#include <stuff_64.h>

InternalCF * imm_mul (InternalCF * lhs, InternalCF * rhs )
{
    int li = imm2int( lhs ), ri = imm2int( rhs );    
    Int_64 pr;
    
    if (li<0)
    {
      if (ri<0)
        mul_64(&pr, -li, -ri);
      else
        mul_64(&pr, -li, ri);
    }
    else
    {
      if (ri<0)
        mul_64(&pr, li, -ri);
      else
        mul_64(&pr, li, ri);
    }
    if (pr.hi || (pr.lo>MAXIMMEDIATE))
    {
	  InternalCF * res = CFFactory::basic( IntegerDomain, li, true );
	  return res->mulcoeff( rhs );
    }
    else
    {
      int res = pr.lo;
      if ((li>0) == (ri>0))
        return int2imm(res);
      else
        return int2imm(-res);
	}
}

#endif
