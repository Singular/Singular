#include "aso.h"
#include "mod2.h"
#include "gmp.h"
#include "structs.h"
main()
{
  ASO(MP_INT);
  ASO(char_ptr);
  ASO(ideal);
  ASO(int);
  ASO(poly);
  ASO(void_ptr);
  ASO(indlist);
  ASO(naIdeal);
  ASO(snaIdeal);
  ASO(sm_prec);
  ASO(smprec);
  ASO(sip_sideal);
  ASO(sip_smap);
  ASO(sip_sring);
  ASO(ip_sideal);
  ASO(ip_smap);
  ASO(ip_sring);
}
