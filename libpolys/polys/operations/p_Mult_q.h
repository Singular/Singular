/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_q.h
 *  Purpose: declaration of some auxillary routines for
 *           p_Mult_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

#ifndef P_MULT_Q_H
#define P_MULT_Q_H

#include "misc/auxiliary.h"
#include "polys/monomials/ring.h"

// Use buckets if min(pLength(p), pLength(q)) >= MIN_LENGTH_BUCKET
// Not thoroughly tested what is best
#ifndef MIN_LENGTH_BUCKET
#define MIN_LENGTH_BUCKET 30
#endif

///  return TRUE and lp == pLength(p), lq == pLength(q),
///              if min(pLength(p), pLength(q)) >= min
///         FALSE if min(pLength(p), pLength(q)) < min
///              and lp >= lq if pLength(p) >= pLength(lq)
///                  lp < lq  if pLength(p) < pLength(q)
BOOLEAN pqLength(poly p, poly q, int &lp, int &lq, const int min);

#endif // P_MULT_Q_H
