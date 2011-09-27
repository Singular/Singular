#ifndef KERNEL_NC_H
#define KERNEL_NC_H
#ifdef HAVE_PLURAL

#include <polys/nc/nc.h>
#include <polys/simpleideals.h>

#include <kernel/polys.h>

/// Compute two-sided GB:
ideal twostd(ideal I);

/// Ann: ???
ideal Approx_Step(ideal L);

class skStrategy;
typedef skStrategy * kStrategy;

// ideal gnc_gr_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy/*, const ring r*/); // Not yet!
ideal gnc_gr_bba (const ideal F, const ideal Q, const intvec *, const intvec *, kStrategy strat/*, const ring r*/);


static inline ideal nc_GB(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat, const ring r)
{
  assume(rIsPluralRing(r));
   
  const ring save = currRing;
   
  if( save != r )
     rChangeCurrRing(r);
   
///////////  rGR->GetNC()->p_Procs.GB          = gnc_gr_bba; // bba even for local case!
// old ///    r->GetNC()->GB()            = gnc_gr_bba;
   
//   rGR->GetNC()->p_Procs.GlobalGB    = gnc_gr_bba;
//   rGR->GetNC()->p_Procs.LocalGB     = gnc_gr_mora;
   
//  assume(r->GetNC()->p_Procs.GB!=NULL);
//  return r->GetNC()->p_Procs.GB(F, Q, w, hilb, strat, r);
  ideal res = gnc_gr_bba(F, Q, w, hilb, strat/*, r*/);
   
   
/*
// Modified Plural's Buchberger's algorithmus.
ideal sca_gr_bba(const ideal F, const ideal Q, const intvec *, const intvec *, kStrategy strat, const ring _currRing);

// Modified modern Sinuglar Buchberger's algorithm.
ideal sca_bba(const ideal F, const ideal Q, const intvec *w, const intvec *, kStrategy strat, const ring _currRing);

// Modified modern Sinuglar Mora's algorithm.
ideal sca_mora(const ideal F, const ideal Q, const intvec *w, const intvec *, kStrategy strat, const ring _currRing);
*/

   
/*
  if (rHasLocalOrMixedOrdering(rGR))
  {
#ifdef PDEBUG
//           Print("Local case => GB == mora!\n");
#endif
    rGR->GetNC()->p_Procs.GB          = NULL; // sca_mora; // local ordering => Mora, otherwise - Buchberger!
  }
  else
  {
#ifdef PDEBUG
//           Print("Global case => GB == bba!\n");
#endif
    rGR->GetNC()->p_Procs.GB          = NULL; // sca_bba; // sca_gr_bba; // sca_bba? // sca_bba;
  }
//   rGR->GetNC()->p_Procs.GlobalGB    = sca_gr_bba;
//   rGR->GetNC()->p_Procs.LocalGB     = sca_mora;

   
/////        r->GetNC()->GB()            = sca_gr_bba;
*/
  if( save != r )
     rChangeCurrRing(save);
   
  return (res);
}
/*

// typedef ideal (*GB_Proc_Ptr)(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat, const ring r);
  GB_Proc_Ptr                           GB;
//                                         GlobalGB, // BBA
//                                         LocalGB;  // MORA
*/


#ifdef PLURAL_INTERNAL_DECLARATIONS

/// we need nc_gr_initBba for sca_gr_bba and gr_bba.
void nc_gr_initBba(ideal F,kStrategy strat); 
#endif // PLURAL_INTERNAL_DECLARATIONS



#endif // HAVE_PLURAL
#endif // KERNEL_NC_H
