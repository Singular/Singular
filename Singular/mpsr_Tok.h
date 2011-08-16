/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_Tok.h
 * Purpose:    Routines which realize Singular CMD <-> MP (dict, cop) mappings
 * Author:     Olaf Bachmann (1/97)
 *
 * Change History (most recent first):
 *
 ***************************************************************/

#ifdef HAVE_MPSR

#ifndef __MPSR_TOK_H__
#define __MPSR_TOK_H__

#include <Singular/mpsr.h>

extern mpsr_Status_t mpsr_tok2mp(short tok, MP_DictTag_t *dict,
                               MP_Common_t *cop);

extern mpsr_Status_t mpsr_mp2tok(MP_DictTag_t dict, MP_Common_t cop,
                               short *o_tok);

extern MP_Common_t mpsr_ord2mp(int sr_ord);
extern short mpsr_mp2ord(MP_Common_t mp_ord);

#define MP_AnnotSingularProcDef         1
#define MP_AnnotSingularPackageType     2

#endif  // __MPSR_TOK_H__


#endif // #ifdef HAVE_MPSR
