/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.h
 *
 * absolute multivariate factorization over Q
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ABS_FACT_H
#define FAC_ABS_FACT_H

#include "facAbsBiFact.h"

#ifdef HAVE_NTL
CFAFList absFactorizeMain (const CanonicalForm& F);
#endif

/*BEGINPUBLIC*/
#ifdef HAVE_NTL
CFAFList absFactorize (const CanonicalForm& G ///<[in] poly over Q
                      );
#endif
/*ENDPUBLIC*/


#endif
