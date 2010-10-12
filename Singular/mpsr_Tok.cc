/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_Tok.cc
 * Purpose:    Routines which realize Singular CMD <-> MP (dict, cop) mappings
 *             (and ordering mappings)
 * Author:     Olaf Bachmann (1/97)
 *
 * Change History (most recent first):
 *
 ***************************************************************/

#include <sys/types.h>
#include <unistd.h>

#include <kernel/mod2.h>

#ifdef HAVE_MPSR

#include <kernel/febase.h>
#include <tok.h>

#include <Singular/mpsr.h>
#include <Singular/mpsr_Tok.h>


#define MAX_COP 256 // there may be at most 256 cops

// this is the main data struct for storing the relation
// Singular token <-> (Dict, OP)
typedef struct mpsr_cmd
{

  short         tok;    // The Singular token encoding

  // The MP Dict tag in which this token is defined,
  MP_DictTag_t  dict;

  // The MP operator corresponding to this token
  MP_Common_t   cop; // operator
} mpsr_cmd;

#define MAX_SR_DICT     5

// this provides a mapping from MP dict tags to more useful (small)
// array indicies
inline short mpdict2srdict(MP_DictTag_t dict)
{
  if (dict == MP_SingularDict) return 0;
  else if (dict == MP_BasicDict) return 1;
  else if (dict == MP_PolyDict) return 2;
  else if (dict == MP_MatrixDict) return 3;
  else if (dict == MP_NumberDict) return 4;
  else return MAX_SR_DICT;
}

// Here are the actual definition of these token tables
#include"mpsr_Tok.inc"


// And here are the main routines which provide the mappings
mpsr_Status_t mpsr_tok2mp(short tok, MP_DictTag_t *dict, MP_Common_t *cop)
{
  short tok_index = tok2mp[tok];

  if (tok_index != MAX_TOK)
  {
    *dict = mpsr_cmds[tok_index].dict;
    *cop = mpsr_cmds[tok_index].cop;
    return mpsr_Success;
  }
  else
    return mpsr_UnknownSingularToken;
}

mpsr_Status_t mpsr_mp2tok(MP_DictTag_t dict, MP_Common_t cop, short *o_tok)
{
  short sr_dict = mpdict2srdict(dict);
  short tok;

  if (sr_dict == MAX_SR_DICT)
    return mpsr_UnknownDictionary;

  tok = mp2tok[sr_dict][cop];
  if (tok == MAX_TOK)
    return mpsr_UnkownOperator;

  *o_tok = tok;
  return mpsr_Success;
}


#define MAX_ORD ringorder_unspec

static struct
{
  int sing_ord;
  int mp_ord;
} sing_mp_ord[] =
{
  {ringorder_no,    MP_CcPolyOrdering_Unknown},
  {ringorder_a,     MP_CcPolyOrdering_Vector},
  {ringorder_c,     MP_CcPolyOrdering_IncComp},
  {ringorder_C,     MP_CcPolyOrdering_DecComp},
  {ringorder_M,     MP_CcPolyOrdering_Matrix},
  {ringorder_lp,    MP_CcPolyOrdering_Lex},
  {ringorder_dp,    MP_CcPolyOrdering_DegRevLex},
  {ringorder_Dp,    MP_CcPolyOrdering_DegLex},
  {ringorder_wp,    MP_CcPolyOrdering_RevLex},
  {ringorder_Wp,    MP_CcPolyOrdering_Lex},
  {ringorder_ls,    MP_CcPolyOrdering_NegLex},
  {ringorder_ds,    MP_CcPolyOrdering_NegDegRevLex},
  {ringorder_Ds,    MP_CcPolyOrdering_NegDegLex},
  {ringorder_ws,    MP_CcPolyOrdering_NegRevLex},
  {ringorder_Ws,    MP_CcPolyOrdering_NegLex},
  {ringorder_unspec, MP_CcPolyOrdering_Unknown}
};

MP_Common_t mpsr_ord2mp(int sr_ord)
{
  int i = ringorder_no;

  while (sing_mp_ord[i].sing_ord != sr_ord &&
         sing_mp_ord[i].sing_ord <= ringorder_unspec) i++;

  return sing_mp_ord[i].mp_ord;
}

short mpsr_mp2ord(MP_Common_t mp_ord)
{
  int ord = ringorder_no;

  while (sing_mp_ord[ord].mp_ord != mp_ord &&
         sing_mp_ord[ord].sing_ord <= ringorder_unspec) ord++;
  return sing_mp_ord[ord].sing_ord;
}

#endif // HAVE_MPSR
