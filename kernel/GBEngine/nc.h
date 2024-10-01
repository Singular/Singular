#ifndef KERNEL_NC_H
#define KERNEL_NC_H

#include "kernel/mod2.h"

#ifdef HAVE_PLURAL

#ifdef PLURAL_INTERNAL_DECLARATIONS

# include "polys/nc/gb_hack.h"

#else // #ifdef PLURAL_INTERNAL_DECLARATIONS

# define PLURAL_INTERNAL_DECLARATIONS
# include "polys/nc/gb_hack.h"
# undef PLURAL_INTERNAL_DECLARATIONS

#endif // #ifdef PLURAL_INTERNAL_DECLARATIONS

#include "polys/nc/nc.h"
#include "polys/simpleideals.h"

#include "kernel/polys.h"



static inline ideal nc_GB(const ideal F, const ideal Q, const intvec *w, const bigintmat *hilb, kStrategy strat, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.GB!=NULL);

  BBA_Proc gb = cast_A_to_B<void*, BBA_Proc>(r->GetNC()->p_Procs.GB);

  // NOTE: the following code block is a hack in order to make a linker to
  // believe in these functions but in reallity it should not be used.
  // Although it can also serve as an illustration for the
  // NC-initialization procedure for GB hidden away (hackedly) in
  // libpolys.
  // The only other solution would be to separate GB and the whole NC
  // subsystems from both libpolys AND kernel... which would require
  // too much effort and thus cannot be done right now.
  // Therefore this is a TODO for a future (large-scale) cleanup.
  if( gb == NULL)
  {
    if( rIsSCA(r) )
    {
      if (rHasLocalOrMixedOrdering(r))
        gb = sca_mora;
      else
        gb = sca_bba; // sca_gr_bba???
    } else
    {
      if (rHasLocalOrMixedOrdering(r))
        gb = gnc_gr_mora;
      else
        gb = gnc_gr_bba;
    }

    r->GetNC()->p_Procs.GB = cast_A_to_vptr(gb);
  }

  return gb(F, Q, w, hilb, strat, r);
}

/// Compute two-sided GB:
ideal twostd(ideal I);

/// Ann: ???
ideal Approx_Step(ideal L);

#endif // HAVE_PLURAL

#endif // KERNEL_NC_H
