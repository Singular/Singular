#ifndef POLYS_NC_GB_HACK_H
#define POLYS_NC_GB_HACK_H

#ifdef HAVE_PLURAL

#ifdef PLURAL_INTERNAL_DECLARATIONS
#include "polys/nc/nc.h"

THREAD_VAR extern NF_Proc nc_NF;
THREAD_VAR extern BBA_Proc gnc_gr_bba, gnc_gr_mora, sca_bba, sca_mora, sca_gr_bba;

/* the realiziation of the routines above in libkernel:*/
ideal k_gnc_gr_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);
ideal k_gnc_gr_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);
/// Modified Plural's Buchberger's algorithmus.
ideal k_sca_gr_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

/// Modified modern Sinuglar Buchberger's algorithm.
ideal k_sca_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

/// Modified modern Sinuglar Mora's algorithm.
ideal k_sca_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

ideal k_sca_gr_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

#endif // PLURAL_INTERNAL_DECLARATIONS

#endif // HAVE_PLURAL
#endif // POLYS_NC_GB_HACK_H

