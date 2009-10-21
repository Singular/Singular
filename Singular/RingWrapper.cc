#include "mod2.h"

#ifdef HAVE_WRAPPERS

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "febase.h"
#include "ipshell.h"
#include "grammar.h"
#include "ipid.h"
#include "polys.h"
#include "Wrappers.h"
#include "RingWrapper.h"

RingWrapper::RingWrapper (const char* ringName, const int characteristic,
                          const int varNumber, const char** varNames, const char* ringOrder)
{
  +prpr > "creating a new RingWrapper (internal type: SINGULAR ring)";
  m_singularRing = (ring) omAlloc0Bin(sip_sring_bin);
  m_singularRing->ch    = characteristic;
  m_singularRing->N     = varNumber;
  m_singularRing->names = (char **) omAlloc0(varNumber * sizeof(char_ptr));
  int i;
  for (i = 0; i < varNumber; i++) m_singularRing->names[i]  = omStrDup(varNames[i]);
  /* weights: entries for 2 blocks: NULL */
  m_singularRing->wvhdl = (int **)omAlloc0(2 * sizeof(int_ptr));
  /* order: ringOrder,0 */
  m_singularRing->order  = (int *) omAlloc(2* sizeof(int *));
  m_singularRing->block0 = (int *)omAlloc0(2 * sizeof(int *));
  m_singularRing->block1 = (int *)omAlloc0(2 * sizeof(int *));
  /* ringorder 'ringOrder' for the first block: var 1..varNumber */
  if (strcmp(ringOrder, "dp") == 0) m_singularRing->order[0]  = ringorder_dp;
  if (strcmp(ringOrder, "lp") == 0) m_singularRing->order[0]  = ringorder_lp;
  if (strcmp(ringOrder, "Dp") == 0) m_singularRing->order[0]  = ringorder_Dp;
  if (strcmp(ringOrder, "ds") == 0) m_singularRing->order[0]  = ringorder_ds;
  if (strcmp(ringOrder, "ls") == 0) m_singularRing->order[0]  = ringorder_ls;
  if (strcmp(ringOrder, "Ds") == 0) m_singularRing->order[0]  = ringorder_Ds;
  m_singularRing->block0[0] = 1;
  m_singularRing->block1[0] = varNumber;
  /* the last block: everything is 0 */
  m_singularRing->order[1]  = 0;
  /* polynomial ring */
  m_singularRing->OrdSgn    = 1;

  /* complete ring intializations */
  rComplete(m_singularRing);
}

RingWrapper::RingWrapper ()
{
  +prpr > "creating a new RingWrapper (internal type: SINGULAR ring)";
  m_singularRing = currRing;
}

RingWrapper::~RingWrapper ()
{
  +prpr > "RingWrapper destructor, object = " < this->toString();
}

char* RingWrapper::toString () const
{
  char* str = new char[200];
  strcpy(str, "");
  int ch = rChar(m_singularRing);
  int n = rVar(m_singularRing);
  if (ch == 0)
    strcat(str, "Q");
  else
  {
    char h[10];
    sprintf(h, "Z/%d", ch);
    strcat(str, h);
  }
  strcat(str, "[");
  for (int i = 0; i < n; i++)
  {
    if (i > 0) strcat(str, ",");
    strcat(str, m_singularRing->names[i]);
  }
  strcat(str, "], ordering = ");
  if (m_singularRing->order[0] == ringorder_dp) strcat(str, "dp");
  if (m_singularRing->order[0] == ringorder_lp) strcat(str, "lp");
  if (m_singularRing->order[0] == ringorder_Dp) strcat(str, "Dp");
  if (m_singularRing->order[0] == ringorder_ds) strcat(str, "ds");
  if (m_singularRing->order[0] == ringorder_ls) strcat(str, "ls");
  if (m_singularRing->order[0] == ringorder_Ds) strcat(str, "Ds");

  return str;
}

void RingWrapper::print () const
{
  PrintS(this->toString());
}

void RingWrapper::printLn () const
{
  PrintS(this->toString());
  PrintLn();
}

const SingularRing& RingWrapper::getSingularRing () const
{
  return m_singularRing;
}

bool RingWrapper::isCompatible (const RingWrapper& r) const
{
  bool result = (this == &r);
  +prpr > "check for ring compatibility, result = " < (result ? "true" : "false");
  return result;
}

#endif
/* HAVE_WRAPPERS */