/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpsr_GetMisc.cc,v 1.4 1997-04-02 15:07:38 Singular Exp $ */

/***************************************************************
 *
 * File:       mpsr_GetMisc.cc
 * Purpose:    Miscellanous routines which are needed by mpsr_Get
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *
 ***************************************************************/

#include "mod2.h"

#ifdef HAVE_MPSR

#include"mpsr_Get.h"
#include "ring.h"
#include "longalg.h"
#include "tok.h"
#include "maps.h"
#include "lists.h"

BOOLEAN currComplete = FALSE;

static ring rDefault(short ch, char *name);
static char* GenerateRingName();


// use the varname so that they are compatible with the ones generated
// by GetRingAnnots
#define MPSR_DEFAULT_VARNAME "x(1)"

// returns some default ring
int mpsr_rDefault(short ch, char *name, ring &r)
{
  // check for currRing
  if (currRing != NULL && currRing->ch == ch)
  {
    int i, n = currRing->N;
    char **names = currRing->names;
    
    for (i=0; i<n; i++)
    {
      if (strcmp(names[i], name) == 0)
      {
        (currRing->ref)++;
        r = currRing;
        return i;
      }
    }
  }
  r = rDefault(ch, name);
  return 0;
}

ring mpsr_rDefault(short ch)
{
  if (currRing != NULL && currRing->ch == ch)
  {
    (currRing->ref)++;
    return currRing;
  }
  else
    return rDefault(ch, MPSR_DEFAULT_VARNAME);
}

static ring rDefault(short ch, char *name)
{
  ring r = (ring) Alloc0(sizeof(sip_sring));
  r->ch = ch;
  r->N = 1;
  r->names = (char **) Alloc(sizeof(char *));
  r->names[0] = mstrdup(name);

  r->wvhdl = (short **)Alloc0(3 * sizeof(short *));
  /*order: dp,C,0*/
  r->order = (int *) Alloc(3 * sizeof(int *));
  r->block0 = (int *)Alloc(3 * sizeof(int *));
  r->block1 = (int *)Alloc(3 * sizeof(int *));
  /* ringorder dp for the first block: var 1..3 */
  r->order[0]  = ringorder_unspec;
  r->block0[0] = 1;
  r->block1[0] = 1;
  /* ringorder C for the second block: no vars */
  r->order[1]  = ringorder_C;
  r->block0[1] = 0;
  r->block1[1] = 0;
  /* the last block: everything is 0 */
  r->order[2]  = 0;
  r->block0[2] = 0;
  r->block1[2] = 0;

  return r;
}

// returns TRUE, if r1 equals r2
// FALSE, otherwise
// Equality is determined componentwise
BOOLEAN mpsr_RingEqual(ring r1, ring r2)
{
  int i, j;

  if (r1 == r2) return 1;

  if (r1 == NULL || r2 == NULL) return 0;

  if ((r1->ch != r2->ch) || (r1->N != r2->N) || (r1->OrdSgn != r2->OrdSgn)
      || (r1->P != r2->P))
    return 0;

  for (i=0; i<r1->N; i++)
    if (strcmp(r1->names[i], r2->names[i])) return 0;

  i=0;
  while (r1->order[i] != 0)
  {
    if (r2->order[i] == 0) return 0;
    if ((r1->order[i] != r2->order[i]) ||
        (r1->block0[i] != r2->block0[i]) || (r2->block0[i] != r1->block0[i]))
      return 0;
    if (r1->wvhdl[i] != NULL)
    {
      if (r2->wvhdl[i] == NULL)
        return 0;
      for (j=0; j<r1->block1[i]-r1->block0[i]+1; j++)
        if (r2->wvhdl[i][j] != r1->wvhdl[i][j])
          return 0;
    }
    else if (r2->wvhdl[i] != NULL) return 0;
    i++;
  }

  for (i=0; i<rPar(r1);i++)
  {
      if (strcmp(r1->parameter[i], r2->parameter[i])!=0)
        return 0;
  }

  if (r1->minpoly != NULL)
  {
    if (r2->minpoly == NULL) return 0;
    mpsr_SetCurrRing(r1, FALSE);
    if (! naEqual(r1->minpoly, r2->minpoly)) return 0;
  }
  else if (r2->minpoly != NULL) return 0;

  if (r1->qideal != NULL)
  {
    ideal id1 = r1->qideal, id2 = r2->qideal;
    int i, n;
    poly *m1, *m2;
    
    if (id2 == NULL) return 0;
    if ((n = IDELEMS(id1)) != IDELEMS(id2)) return 0;

    mpsr_SetCurrRing(r1, FALSE);
    m1 = id1->m;
    m2 = id2->m;
    for (i=0; i<n; i++)
      if (! pEqualPolys(m1[i],m2[i])) return 0;
  }
  else if (r2->qideal != NULL) return 0;

  return 1;
}

  
// returns TRUE, if r1 less or equals r2
// FALSE, otherwise
// Less or equal means that r1 is a strong subring of r2
inline BOOLEAN RingLessEqual(ring r1, ring r2)
{
  int i, j;

  if (r1 == r2) return 1;

  if (r1 == NULL) return 1;

  if (r2 == NULL) return 0;

  if ((r1->N > r2->N) || (r1->OrdSgn != r2->OrdSgn) || (r1->P > r2->P))
    return 0;

  if (r1->ch != 0 && r1->ch != r2->ch) return 0;

  for (i=0, j=0; j<r1->N && i<r2->N; i++)
    if (strcmp(r1->names[j], r2->names[i]) == 0) j++;
  if (j < r1->N) return 0;

  // for ordering, suppose that they are only simple orderings
  if (r1->order[2] != 0 || r2->order[2] != 0 ||
      (r1->order[0] != r2->order[0] && r1->order[0] != ringorder_unspec) ||
      r1->order[1] != r2->order[1])
    return 0;
  
  for (i=0; i<r1->P;i++)
  {
      if (strcmp(r1->parameter[i], r2->parameter[i])!=0)
        return 0;
  }
  // r1->parameter == NULL && r2->parameter != NULL  is ok

  if (r1->minpoly != NULL)
  {
    if (r2->minpoly == NULL) return 0;
    mpsr_SetCurrRing(r1, FALSE);
    if (! naEqual(r1->minpoly, r2->minpoly)) return 0;
  }
  return 1;
}

// returns MP_Success and lv2 appended to lv1, both over the same ring,
// or MP_Failure
mpsr_Status_t mpsr_MergeLeftv(mpsr_leftv mlv1, mpsr_leftv mlv2)
{
  ring r, r1 = mlv1->r, r2 = mlv2->r;
  leftv lv;

  if (mpsr_RingEqual(r1,r2))
  {
    if (r2 != NULL) rKill(r2);
    r = r1;
  }
  else if (RingLessEqual(r1, r2))
  {
    r = r2;
    if (r1 != NULL)
    {
      mpsr_MapLeftv(mlv1->lv, r1, r);
      rKill(r1);
    }
  }
  else if (RingLessEqual(r2, r1))
  {
    r = r1;
    if (r2 != NULL)
    {
      mpsr_MapLeftv(mlv2->lv, r2, r);
      rKill(r2);
    }
  }
  else if (rSum(r1, r2, r) >= 0)
  {
    mpsr_MapLeftv(mlv1->lv, r1, r);
    mpsr_MapLeftv(mlv2->lv, r2, r);
    rKill(r1);
    rKill(r2);
  }
  else return mpsr_Failure;

  lv = mlv1->lv;
  while (lv->next != NULL) lv = lv->next;
  
  lv->next = mlv2->lv;
  mlv1->r = r;

  // this is an optimization for the mpsr_rDefault routines
  currRing = r;
  return mpsr_Success;
}

void mpsr_MapLeftv(leftv l, ring from_ring, ring to_ring)
{
  int i, n;
  
  while (l != NULL)
  {
    short typ = l->Typ();
    
    switch(typ)
    {
        case POLY_CMD:
        case VECTOR_CMD:
        {
          poly p = (poly) l->data;
          mpsr_SetCurrRing(to_ring, TRUE);
          l->data = (void *) maIMap(from_ring, (poly) l->data);
          ppDelete(&p, from_ring);
          break;
        }

        case MODUL_CMD:
        case IDEAL_CMD:
        case MATRIX_CMD:
        case MAP_CMD:
        {
          ideal id = (ideal) l->Data();
          n = IDELEMS(id);
          poly *m = id->m, *m1 = (poly *) Alloc(n*sizeof(poly));
          mpsr_SetCurrRing(to_ring, TRUE);
          for (i=0; i<n; i++)
          {
            m1[i] = m[i];
            m[i] = maIMap(from_ring, m[i]);
          }
          mpsr_SetCurrRing(from_ring, FALSE);
          for (i=0; i<n; i++) pDelete(&(m1[i]));
          Free(m1, n*sizeof(poly));
          break;
        }
          
        case LIST_CMD:
        {
          lists ll = (lists) l->Data();
          n = ll->nr + 1;
          for (i=0; i<n; i++) mpsr_MapLeftv(&(ll->m[i]), from_ring, to_ring);
          break;
        }

        case COMMAND:
        {
          command cmd = (command) l->Data();
          if (cmd->op == PROC_CMD && cmd->argc == 2)
            mpsr_MapLeftv(&(cmd->arg2), from_ring, to_ring);
          else if (cmd->argc > 0)
          {
            mpsr_MapLeftv(&(cmd->arg1), from_ring, to_ring);
            if (cmd->argc > 1)
            {
              mpsr_MapLeftv(&(cmd->arg2), from_ring, to_ring);
              if (cmd->argc > 2)
                mpsr_MapLeftv(&(cmd->arg3), from_ring, to_ring);
            }
          }
          break;
        }

        case NUMBER_CMD:
        {
          number nn = (number) l->data;
          mpsr_SetCurrRing(to_ring, TRUE);
          nSetMap(from_ring->ch, from_ring->parameter, from_ring->P, from_ring->minpoly);
          l->data = (void *) nMap(nn);
          mpsr_SetCurrRing(from_ring, FALSE);
          nDelete(&nn);
        }
    }
    l = l->next;
  }
}
          
           
// searches for a ring handle which has a ring which is equal to r
// if one is found, then this one is set to the new global ring
// otherwise, a ring name is generated, and a new idhdl is created
void mpsr_SetCurrRingHdl(ring r)
{
  idhdl h = idroot, rh = NULL;

  if (r == NULL)
  {
    if (currRingHdl != NULL && currRing != IDRING(currRingHdl))
      mpsr_SetCurrRing(IDRING(currRingHdl), TRUE);
    return;
  }
  
  // try to find an idhdl which is an equal ring
  while (h != NULL)
  {
    if ((IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD) &&
        (mpsr_RingEqual(IDRING(h), r)))
    {
      // found one
      rh = h;
      break;
    }
    h = h->next;
  }

  if (rh != NULL)
  {
    // found an idhdl to an equal ring
    // we better reset currRing, so that rSetHdl does not choke (see
    // sLastPrinted)
    if (currRingHdl != NULL && IDRING(currRingHdl) != currRing)
      mpsr_SetCurrRing(IDRING(currRingHdl), TRUE);

    rSetHdl(rh, TRUE);

    if (currRing != r)
    {
      mpsr_assume(r->ref <= 0);
      rKill(r);
    }
  }
  else
  {
    rh = mpsr_InitIdhdl((r->qideal == NULL ? (short) RING_CMD
                         : (short) QRING_CMD),
                        (void *) r, GenerateRingName());
    // reset currRing for reasons explained above
    if (currRingHdl != NULL) mpsr_SetCurrRing(IDRING(currRingHdl), TRUE);
    rSetHdl(rh, TRUE);
    rh->next = idroot;
    idroot = rh;
    r->ref = 0;
  }
}


int gringcounter = 0;
char grname[14];

static char* GenerateRingName()
{
  sprintf(grname, "mpsr_r%d", gringcounter++);
  return grname;
}
        
// searches through the Singular namespace for a matching name:
// the first found is returned together witht the respective ring
idhdl mpsr_FindIdhdl(char *name, ring &r)
{
  idhdl h = idroot->get(name, 0), h2;
  r = NULL;
  
  if (h != NULL)
  {
    r = NULL;
    return h;
  }

  h = idroot;
  while ( h != NULL)
  {
    if (IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD)
    {
      h2 = IDRING(h)->idroot->get(name, 0);
      if (h2 != NULL)
      {
        r = IDRING(h);
        r->ref++;
        return h2;
      }
    }
    h = h->next;
  }
  return NULL;
}


/***************************************************************
 *
 * Stuff which deals with External Data
 *
 ***************************************************************/

void mpsr_DeleteExternalData(MPT_ExternalData_t edata)
{
  mpsr_leftv mlv = (mpsr_leftv) edata;

  if (edata != NULL)
  {
    if (mlv->r != NULL) mpsr_SetCurrRing(mlv->r, FALSE);
    if (mlv->lv != NULL)
    {
      mlv->lv->CleanUp();
      Free(mlv->lv, sizeof(sleftv));
    }
    if (mlv->r != NULL) rKill(mlv->r);
  }
  Free(mlv, sizeof(mpsr_sleftv));
}

void mpsr_CopyExternalData(MPT_ExternalData_t *dest,
                           MPT_ExternalData_t src)
{
  mpsr_leftv slv = (mpsr_leftv) src, dlv;

  if (slv != NULL)
  {
    dlv = (mpsr_leftv) Alloc0(sizeof(mpsr_sleftv));
    dlv->r = rCopy(slv->r);
    dlv->lv = (leftv) Alloc0(sizeof(sleftv));
    if (slv->lv != NULL) dlv->lv->Copy(slv->lv);
    else dlv->lv = NULL;

    *dest = (MPT_ExternalData_t) dlv;
  }
  else
    *dest = NULL;
}

/***************************************************************
 *
 * mpsr initialization
 *
 ***************************************************************/

#ifdef MDEBUG
void * mpAllocBlock( size_t t)
{
  return mmDBAllocBlock(t,"mp",0);
}
void mpFreeBlock( void* a, size_t t)
{
  mmDBFreeBlock(a,t,"mp",0);
}

void * mpAlloc( size_t t)
{
  return mmDBAlloc(t,"mp",0);
}
void mpFree(void* a)
{
  mmDBFree(a,"mp",0);
}
#endif

void mpsr_Init()
{
  // memory management functions of MP (and MPT)
#ifndef MDEBUG
  IMP_RawMemAllocFnc = mmAlloc;
  IMP_RawMemFreeFnc = mmFree;
  IMP_MemAllocFnc = mmAllocBlock;
  IMP_MemFreeFnc = mmFreeBlock;
#else
  IMP_RawMemAllocFnc = mpAlloc;
  IMP_RawMemFreeFnc = mpFree;
  IMP_MemAllocFnc = mpAllocBlock;
  IMP_MemFreeFnc = mpFreeBlock;
#endif

  // Init of the MPT External Data functions
  MPT_GetExternalData = mpsr_GetExternalData;
  MPT_DeleteExternalData = mpsr_DeleteExternalData;

#ifdef  PARI_BIGINT_TEST
  init(4000000, 2);
#endif  
}

#ifdef MPSR_DEBUG
// this is just a dummy function, where we can set a debugger breakpoint
void mpsr_Break()
{
  Werror("mpsr_Error");
}
#endif

#endif // HAVE_MPSR
