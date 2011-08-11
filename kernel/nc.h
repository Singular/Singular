#ifndef KERNEL_NC_H
#define KERNEL_NC_H
#ifdef HAVE_PLURAL

#include <polys/nc/nc.h>
#include <polys/simpleideals.h>



/// Compute two-sided GB:
ideal twostd(ideal I);

/// Ann: ???
ideal Approx_Step(ideal L);


#ifdef PLURAL_INTERNAL_DECLARATIONS

/// we need nc_gr_initBba for sca_gr_bba and gr_bba.
void nc_gr_initBba(ideal F,kStrategy strat); 
#endif // PLURAL_INTERNAL_DECLARATIONS



#endif // HAVE_PLURAL
#endif // KERNEL_NC_H