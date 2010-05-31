/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_q.h
 *  Purpose: declaration of some auxillary routines for
 *           p_Mult_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#include <kernel/mod2.h>
#include <kernel/structs.h>

// Use buckets if min(pLength(p), pLength(q)) >= MIN_LENGTH_BUCKET
// Not thoroughly tested what is best
#define MIN_LENGTH_BUCKET 10

//  return TRUE and lp == pLength(p), lq == pLength(q),
//              if min(pLength(p), pLength(q)) >= min
//         FALSE if min(pLength(p), pLength(q)) < min
//              and lp >= lq if pLength(p) >= pLength(lq)
//                  lp < lq  if pLength(p) < pLength(q)
BOOLEAN pqLength(poly p, poly q, int &lp, int &lq, const int min);

