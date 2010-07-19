/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_GetMisc.cc
 * Purpose:    Miscellanous routines which are needed by mpsr_Get
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *
 ***************************************************************/

#include <Singular/mod2.h>

#ifdef HAVE_MPSR

#include <Singular/mpsr_Get.h>
#include "longalg.h"
#include <Singular/tok.h>
#include <kernel/maps.h>
#include <Singular/lists.h>

BOOLEAN currComplete = FALSE;

static ring mpsr_rDefault(short ch, const char *name);
static char* GenerateRingName();


// use the varname so that they are compatible with the ones generated
// by GetRingAnnots
#define MPSR_DEFAULT_VARNAME "x(1)"

// returns some default ring
int mpsr_rDefault(short ch, const char *name, ring &r)
{
  // check for currRing
  if (currRing != NULL && rInternalChar(currRing) == ch)
  // orig: currRing->ch==ch ???
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
  r = mpsr_rDefault(ch, name);
  return 0;
}

ring mpsr_rDefault(short ch)
{
  if (currRing != NULL && rInternalChar(currRing) == ch)
  // orig: currRing->ch==ch ???
  {
    (currRing->ref)++;
    return currRing;
  }
  else
    return mpsr_rDefault(ch, MPSR_DEFAULT_VARNAME);
}

static ring mpsr_rDefault(short ch, const char *name)
{
  ring r = (ring) omAlloc0Bin(sip_sring_bin);
  r->ch = ch;
  r->N = 1;
  r->names = (char **) omAlloc0Bin(char_ptr_bin);
  r->names[0] = omStrDup(name);

  r->wvhdl = (int **)omAlloc0(3 * sizeof(int *));
  /*order: dp,C,0*/
  r->order = (int *) omAlloc(3 * sizeof(int *));
  r->block0 = (int *)omAlloc(3 * sizeof(int *));
  r->block1 = (int *)omAlloc(3 * sizeof(int *));
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
  rComplete(r);
  return r;
}

// returns TRUE, if r1 less or equals r2
// FALSE, otherwise
// Less or equal means that r1 is a strong subring of r2
static inline BOOLEAN RingLessEqual(ring r1, ring r2)
{
  int i, j;

  if (r1 == r2) return 1;

  if (r1 == NULL) return 1;

  if (r2 == NULL) return 0;

  if ((r1->N > r2->N) || (r1->OrdSgn != r2->OrdSgn) || (rPar(r1) > rPar(r2)))
    return 0;

  if (!rField_is_Q(r1) && rInternalChar(r1) != rInternalChar(r2)) return 0;
  // orig: if (r1->ch != 0 && r1->ch != r2->ch) return 0;

  for (i=0, j=0; j<r1->N && i<r2->N; i++)
    if (strcmp(r1->names[j], r2->names[i]) == 0) j++;
  if (j < r1->N) return 0;

  // for ordering, suppose that they are only simple orderings
  if (r1->order[2] != 0 || r2->order[2] != 0 ||
      (r1->order[0] != r2->order[0] && r1->order[0] != ringorder_unspec) ||
      r1->order[1] != r2->order[1])
    return 0;

  for (i=0; i<rPar(r1);i++)
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

  if (rEqual(r1,r2))
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

#ifdef RDEBUG
  if (r!= NULL) rTest(r);
#endif
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
          p_Delete(&p, from_ring);
          break;
        }

        case MODUL_CMD:
        case IDEAL_CMD:
        case MATRIX_CMD:
        case MAP_CMD:
        {
          ideal id = (ideal) l->Data();
          n = IDELEMS(id);
          poly *m = id->m, *m1 = (poly *) omAlloc(n*sizeof(poly));
          mpsr_SetCurrRing(to_ring, TRUE);
          for (i=0; i<n; i++)
          {
            m1[i] = m[i];
            m[i] = maIMap(from_ring, m[i]);
          }
          mpsr_SetCurrRing(from_ring, FALSE);
          for (i=0; i<n; i++) pDelete(&(m1[i]));
          omFreeSize(m1, n*sizeof(poly));
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
          nMapFunc nMap=nSetMap(from_ring);
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
void mpsr_SetCurrRingHdl(mpsr_leftv mlv)
{
  idhdl h = IDROOT, rh = NULL;
  ring r = mlv->r;

  if (r == NULL)
  {
    if (currRingHdl != NULL && currRing != IDRING(currRingHdl))
      mpsr_SetCurrRing(IDRING(currRingHdl), TRUE);
    return;
  }

  rTest(r);
  // try to find an idhdl which is an equal ring
  while (h != NULL)
  {
    if ((IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD) &&
        (rEqual(IDRING(h), r)))
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

    // reset debug field so that RingOfLm does not complain
    // this does nothing on !PDEBUG
    p_SetRingOfLeftv(mlv->lv, IDRING(rh));

    rSetHdl(rh);

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
    rSetHdl(rh);
    rh->next = IDROOT;
    IDROOT = rh;
    r->ref = 0;
  }
}


static int gringcounter = 0;
static char grname[14];

static char* GenerateRingName()
{
  sprintf(grname, "mpsr_r%d", gringcounter++);
  return grname;
}

// searches through the Singular namespace for a matching name:
// the first found is returned together witht the respective ring
idhdl mpsr_FindIdhdl(char *name, ring &r)
{
  idhdl h = (IDROOT != NULL ? IDROOT->get(name, 0): (idhdl) NULL), h2;
  r = NULL;

  if (h != NULL)
  {
    r = NULL;
    return h;
  }

  h = IDROOT;
  while ( h != NULL)
  {
    if (IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD)
    {
      h2 = (IDRING(h)->idroot!=NULL ?IDRING(h)->idroot->get(name, 0) :
            (idhdl) NULL);

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
      omFreeBin(mlv->lv, sleftv_bin);
    }
    if (mlv->r != NULL) rKill(mlv->r);
  }
  omFreeBin(mlv, mpsr_sleftv_bin);
}

void mpsr_CopyExternalData(MPT_ExternalData_t *dest,
                           MPT_ExternalData_t src)
{
  mpsr_leftv slv = (mpsr_leftv) src, dlv;

  if (slv != NULL)
  {
    dlv = (mpsr_leftv) omAlloc0Bin(mpsr_sleftv_bin);
    dlv->r = rCopy(slv->r);
    dlv->lv = (leftv) omAlloc0Bin(sleftv_bin);
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
#undef malloc
#undef free
#undef freeSize

#include "mmalloc.h"

static int mpsr_is_initialized = 0;

LINKAGE void mpsr_Init()
{
  if (mpsr_is_initialized) return;
  // memory management functions of MP (and MPT)
#if defined(OMALLOC_USES_MALLOC) || defined(X_OMALLOC)
  IMP_RawMemAllocFnc = omMallocFunc;
  IMP_RawMemFreeFnc = omFreeFunc;
  IMP_MemAllocFnc = omMallocFunc;
  IMP_MemFreeFnc = omFreeSizeFunc;
#else
  IMP_RawMemAllocFnc = malloc;
  IMP_RawMemFreeFnc = free;
  IMP_MemAllocFnc = malloc;
  IMP_MemFreeFnc = freeSize;
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
