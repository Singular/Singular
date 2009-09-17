#include "mod2.h"
#include "ipshell.h"
#include "grammar.h"
#include "ipid.h"
#include "polys.h"
#include "Poly.h"
#include "StringPoly.h"
#include "CanonicalPoly.h"
#include "ObjectFactory.h"

using namespace std;

Poly ObjectFactory::newStringPoly(const string& polyString)
{
  cout << "\n      creating a new Poly (internal type: StringPoly)";
  StringPoly* p_tsp = new StringPoly(polyString);
  return Poly(p_tsp);
}

Poly ObjectFactory::newCanonicalPoly(const int i, const ring r)
{
  cout << "\n      creating a new Poly (internal type: CanonicalPoly)";
  CanonicalPoly* p_cp = new CanonicalPoly(i, r);
  return Poly(p_cp);
}

ring ObjectFactory::newRing(const char* ringName, const int characteristic,
                            const int varNumber, const char** varNames,
                            const char* ringOrder)
{
  idhdl tmp=NULL;

  if (ringName!=NULL) tmp = enterid(ringName, myynest, RING_CMD, &IDROOT);
  if (tmp==NULL) return NULL;

  if (ppNoether!=NULL) pDelete(&ppNoether);
  if (sLastPrinted.RingDependend())
  {
    sLastPrinted.CleanUp();
    memset(&sLastPrinted,0,sizeof(sleftv));
  }

  ring r = IDRING(tmp);

  r->ch    = characteristic;
  r->N     = varNumber;
  /*r->P     = 0; Alloc0 in idhdl::set, ipid.cc*/
  /*names*/
  r->names = (char **) omAlloc0(varNumber * sizeof(char_ptr));
  for (int i = 0; i < varNumber; i++) {
    r->names[i]  = omStrDup(varNames[i]);
  }
  /*weights: entries for 3 blocks: NULL*/
  r->wvhdl = (int **)omAlloc0(3 * sizeof(int_ptr));
  /*order: dp,C,0*/
  r->order = (int *) omAlloc(3 * sizeof(int *));
  r->block0 = (int *)omAlloc0(3 * sizeof(int *));
  r->block1 = (int *)omAlloc0(3 * sizeof(int *));

  /* ringorder for the first block: var 1..3 */
  r->order[0] = ringorder_dp; // default setting
  if (strcmp(ringOrder, "lp") == 0) r->order[0] = ringorder_lp;
  if (strcmp(ringOrder, "Dp") == 0) r->order[0] = ringorder_Dp;
  if (strcmp(ringOrder, "ds") == 0) r->order[0] = ringorder_ds;
  if (strcmp(ringOrder, "ls") == 0) r->order[0] = ringorder_ls;
  if (strcmp(ringOrder, "Ds") == 0) r->order[0] = ringorder_Ds;

  r->block0[0] = 1;
  r->block1[0] = 3;
  /* ringorder C for the second block: no vars */
  r->order[1]  = ringorder_C;
  /* the last block: everything is 0 */
  r->order[2]  = 0;
  /*polynomial ring*/
  r->OrdSgn    = 1;

  /* complete ring intializations */
  rComplete(r);
  rSetHdl(tmp);
  return r;
}
