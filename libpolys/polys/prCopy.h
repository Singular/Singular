#ifndef PRCOPY_H
#define PRCOPY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - declarations of functions for Copy/Move/Delete for Polys
*/

struct spolyrec; typedef struct spolyrec polyrec; typedef polyrec* poly;
struct ip_sring; typedef struct ip_sring* ring; typedef struct ip_sring const* const_ring;
struct sip_sideal; typedef struct sip_sideal *ideal;

struct snumber; typedef struct snumber *   number;
struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;

typedef number (*nMapFunc)(number a, const coeffs src, const coeffs dst);

/*************************************************************************
 *
 * MoveR, CopyR, ShallowCopyR: operations to get ideals/polys
 *                             from source_r to dest_r where
 *  - Coeff(source_r) == Coeff(dest_r)
 *  - dest_r->N <= source_r->N
 * Move:        input is destroyed
 * ShallowCopy: monomials are copied, coeffs are set
 * Copy:        monomials and coeffs are copied
 *
 ************************************************************************/
poly prMoveR_NoSort(poly &p, ring r, ring dest_r);
poly prMoveR(poly &p, ring r, ring dest_r);
poly prCopyR_NoSort(poly p, ring r, ring dest_r);
poly prCopyR(poly p, ring r, ring dest_r);
poly prShallowCopyR_NoSort(poly p, ring r, ring dest_t);
poly prShallowCopyR(poly p, ring r, ring dest_t);
poly prHeadR(poly p, ring r, ring dest_rg);

ideal idrMoveR_NoSort(ideal &id, ring r, ring dest_r );
ideal idrMoveR(ideal &id, ring r, ring dest_r );

ideal idrCopyR_NoSort(ideal id, ring r, ring dest_r );
ideal idrCopyR(ideal id, ring r, ring dest_r );

ideal idrShallowCopyR_NoSort(ideal id, ring r, ring dest_r );
ideal idrShallowCopyR(ideal id, ring r, ring dest_r );

/// Copy leading terms of id[i] via prHeeadR into dest_r
ideal idrHeadR(ideal id, ring r, ring dest_r );

poly prMapR(poly src, nMapFunc nMap, ring src_r, ring dest_r);
#endif
