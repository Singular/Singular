#ifndef POLYS_NC_H
#define POLYS_NC_H

/*

// TODO: the following is a part of ring.h... would be nice to have a
// clear public NC interface defined here!
#ifdef HAVE_PLURAL
struct nc_struct;
typedef struct nc_struct   nc_struct;
#endif

#inculde <polys/monomials/ring.h>

/// we must always have this test!
static inline bool rIsPluralRing(const ring r)
{
#ifdef HAVE_PLURAL
  nc_struct *n;
  return (r != NULL) && ((n=r->GetNC()) != NULL) /*&& (n->type != nc_error)*/;
#else
  return false;
#endif
}

*/
#endif /* POLYS_NC_H */
