/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/* $Id$ */
/***************************************************************
 *
 * File:       mpsr_Get.cc
 * Purpose:    definition of the main Get routine(s)
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *
 ***************************************************************/

#include <Singular/mod2.h>

#ifdef HAVE_MPSR


#include"mpsr_Get.h"
#include"mpsr_Tok.h"

#include <Singular/tok.h>
#include <kernel/longrat.h>
#include <kernel/intvec.h>
#include <kernel/ideals.h>
#include <kernel/matpol.h>
#include <Singular/lists.h>

#include <mylimits.h>

omBin mpsr_sleftv_bin = omGetSpecBin(sizeof(mpsr_sleftv));

/***************************************************************
 *
 * prototypes
 *
 ***************************************************************/
static mpsr_Status_t GetIntVecLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv);
static mpsr_Status_t GetIntMatLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv);
static mpsr_Status_t GetRingLeftv(MP_Link_pt link, MPT_Node_pt node,
                                mpsr_leftv mlv);
static mpsr_Status_t GetPolyLeftv(MP_Link_pt link, MPT_Node_pt node,
                                mpsr_leftv mlv, BOOLEAN IsUnOrdered);
static mpsr_Status_t GetPolyVectorLeftv(MP_Link_pt link, MPT_Node_pt node,
                                        mpsr_leftv mlv,
                                        BOOLEAN IsUnOrdered = FALSE);
static mpsr_Status_t GetIdealLeftv(MP_Link_pt link, MPT_Node_pt node,
                                 mpsr_leftv mlv, BOOLEAN IsUnOrdered = FALSE);
static mpsr_Status_t GetModuleLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv, BOOLEAN IsUnOrdered = FALSE);
static mpsr_Status_t GetMatrixLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv, BOOLEAN IsUnOrdered = FALSE);
static mpsr_Status_t GetMapLeftv(MP_Link_pt link, MPT_Node_pt node,
                               mpsr_leftv mlv);
static mpsr_Status_t GetCopCommandLeftv(MP_Link_pt link, MPT_Node_pt node,
                                      mpsr_leftv mlv, short quote);

/***************************************************************
 *
 * Inlines
 *
 ***************************************************************/

//
// Predicates which examine nodes for primitive Singular types
//
#define fr(cond)    if (! (cond)) return FALSE

static inline BOOLEAN IsIntVecNode(MPT_Node_pt node)
{
  fr(NodeCheck(node, MP_MatrixDict, MP_CopMatrixDenseVector));

  MPT_Tree_pt tree = MPT_ProtoAnnotValue(node);
  return tree != NULL && NodeCheck(tree->node, MP_CommonMetaType,
                                   MP_ProtoDict, MP_CmtProtoIMP_Sint32);
}

static inline BOOLEAN IsIntMatNode(MPT_Node_pt node)
{
  fr(NodeCheck(node, MP_MatrixDict, MP_CopMatrixDenseMatrix));

  MPT_Tree_pt tree = MPT_ProtoAnnotValue(node);
  return tree != NULL && NodeCheck(tree->node, MP_CommonMetaType,
                                   MP_ProtoDict, MP_CmtProtoIMP_Sint32);
}

static inline BOOLEAN IsRingNode(MPT_Node_pt node, ring &r, BOOLEAN &IsUnOrdered)
{
  BOOLEAN mv;
  return
    NodeCheck(node, MP_PolyDict, MP_CopPolyRing) &&
    mpsr_GetRingAnnots(node, r, mv, IsUnOrdered) == mpsr_Success;
}

static inline BOOLEAN IsPolyNode(MPT_Node_pt node, ring &r, BOOLEAN &IsUnOrdered)
{
  BOOLEAN mv;

  //for the timne being, we only accept DDP's
  return
    NodeCheck(node, MP_PolyDict, MP_CopPolyDenseDistPoly) &&
    MPT_Annot(node, MP_PolyDict, MP_AnnotPolyModuleVector) == NULL &&
    mpsr_GetRingAnnots(node, r, mv, IsUnOrdered) == mpsr_Success;
}

static inline BOOLEAN IsPolyVectorNode(MPT_Node_pt node, ring &r,
                                BOOLEAN &IsUnOrdered)
{
  BOOLEAN mv;

  //for the timne being, we only accept DDP's
  return
    NodeCheck(node, MP_PolyDict, MP_CopPolyDenseDistPoly) &&
    MPT_Annot(node, MP_PolyDict, MP_AnnotPolyModuleVector) != NULL &&
    mpsr_GetRingAnnots(node, r, mv, IsUnOrdered) == mpsr_Success;
}

static inline BOOLEAN IsIdealNode(MPT_Node_pt node, ring &r,
                           BOOLEAN &IsUnOrdered)
{
  fr(NodeCheck(node, MP_PolyDict, MP_CopPolyIdeal));
  MPT_Tree_pt tree = MPT_ProtoAnnotValue(node);
  fr(tree != NULL);
  node = tree->node;
  return
    node->type == MP_CommonMetaOperatorType &&
    node->numchild == 0 &&
    IsPolyNode(node, r, IsUnOrdered);
}

static inline BOOLEAN IsModuleNode(MPT_Node_pt node, ring &r, BOOLEAN &IsUnOrdered)
{
  fr(NodeCheck(node, MP_PolyDict, MP_CopPolyModule));
  MPT_Tree_pt tree = MPT_ProtoAnnotValue(node);
  fr(tree != NULL);
  node = tree->node;
  return
    node->type == MP_CommonMetaOperatorType &&
    node->numchild == 0 &&
    IsPolyVectorNode(node, r, IsUnOrdered);
}

static inline BOOLEAN IsMatrixNode(MPT_Node_pt node, ring &r, BOOLEAN &IsUnOrdered)
{
  fr(NodeCheck(node, MP_MatrixDict, MP_CopMatrixDenseMatrix));
  MPT_Tree_pt tree = MPT_ProtoAnnotValue(node);
  fr(tree != NULL);
  node = tree->node;
  return
    node->type == MP_CommonMetaOperatorType &&
    node->numchild == 0 &&
    IsPolyNode(node, r, IsUnOrdered);
}

static inline BOOLEAN IsQuitNode(MPT_Node_pt node)
{
  return NodeCheck(node, MP_MpDict, MP_CopMpEndSession);
}

//
// Init*Leftv functions
//
static inline void InitIntLeftv(mpsr_leftv mlv, int i)
{
  mlv->lv = mpsr_InitLeftv(INT_CMD, (void *) i);
}

static inline void InitApIntLeftv(mpsr_leftv mlv, mpz_ptr apint)
{
  number n = (number) omAllocBin(rnumber_bin);
#if defined(LDEBUG)
    n->debug=123456;
#endif
  mlv->r = mpsr_rDefault(0);
  n->s = 3;
  memcpy(&(n->z), apint, sizeof(mpz_t));
  nlNormalize(n);
  omFreeSize(apint, sizeof(mpz_t));
  mlv->lv = mpsr_InitLeftv(NUMBER_CMD, n);
}

static inline void InitReal32Leftv(mpsr_leftv mlv, MPT_Arg_t r32)
{
  number n = (number) r32;
//  n = Real32_2_Number(r32);

  mlv->r = mpsr_rDefault(-1);
  mlv->lv = mpsr_InitLeftv(NUMBER_CMD, n);
}

static inline void InitIdentifierLeftv(mpsr_leftv mlv, char *name, short quote)
{
  int pos;

  if (quote <= 0)
  {
    idhdl h = mpsr_FindIdhdl(name, mlv->r);

    if (h != NULL)
    {
      mlv->lv = mpsr_InitLeftv(IDHDL, (void *) h);
    }
    else if (strcmp(name, "basering") == 0)
    {
      mlv->lv = mpsr_InitLeftv(IDHDL, (void*) currRingHdl);
      mlv->lv->name=IDID(currRingHdl);
      mlv->lv->attribute=IDATTR(currRingHdl);
    }
    else
    {
      poly p;

      pos = mpsr_rDefault(0, name, mlv->r);
      mpsr_SetCurrRing(mlv->r, TRUE);
      p = pOne();
      pSetExp(p,pos+1, 1);
      pSetm(p);
      mlv->lv = mpsr_InitLeftv(POLY_CMD, (void *) p);
    }
    omFree(name);
  }
  else
  {
    mlv->lv = mpsr_InitLeftv(DEF_CMD, NULL);
    mlv->lv->name = name;
  }
}

/* primitive mpsr_Get*Leftv functions */

static inline mpsr_Status_t mpsr_GetIntLeftv(MPT_Node_pt node, mpsr_leftv mlv)
{
  mpsr_assume(MP_IsFixedIntegerType(node->type));
  mlv->lv = mpsr_InitLeftv(INT_CMD, node->nvalue);
  return mpsr_Success;
}

static inline mpsr_Status_t mpsr_GetReal32Leftv(MPT_Node_pt node, mpsr_leftv mlv)
{
  mpsr_assume(node->type == MP_Real32Type);
  InitReal32Leftv(mlv, node->nvalue);
  return mpsr_Success;
}

static inline mpsr_Status_t mpsr_GetApIntLeftv(MPT_Node_pt node, mpsr_leftv mlv)
{
  InitApIntLeftv(mlv, (mpz_ptr)  node->nvalue);
  node->nvalue = NULL;
  return mpsr_Success;
}

static inline mpsr_Status_t mpsr_GetIdentifierLeftv(MPT_Node_pt node, mpsr_leftv mlv,
                                             short quote)
{
  mpsr_assume(MP_IsIdType(node->type));
  char *id;
  MPT_Annot_pt proc_annot = MPT_Annot(node, MP_SingularDict,
                                      MP_AnnotSingularProcDef);

  if (node->type == MP_CommonGreekIdentifierType ||
      node->type == MP_CommonGreekIdentifierType)
  {
    id  = (char *) omAlloc(2*sizeof(char));
    id[1] = '\0';
    id[0] = MP_UINT8_T(node->nvalue);
  }
  else
  {
    id = MP_STRING_T(node->nvalue);
    node->nvalue = NULL;
  }

  InitIdentifierLeftv(mlv, id, quote);

  if (proc_annot != NULL) mlv->lv->rtyp = PROC_CMD;

  return mpsr_Success;
}

static inline mpsr_Status_t mpsr_GetStringLeftv(MPT_Node_pt node, mpsr_leftv mlv)
{
  mpsr_assume(node->type == MP_StringType);
  mlv->lv = mpsr_InitLeftv(STRING_CMD, MP_STRING_T(node->nvalue));
  node->nvalue = NULL;
  return mpsr_Success;
}

static inline mpsr_Status_t GetQuitLeftv(mpsr_leftv mlv)
{
  mlv->lv = mpsr_InitLeftv(STRING_CMD, (void *) omStrDup(MPSR_QUIT_STRING));
  return mpsr_Success;
}

/***************************************************************
 *
 * The top-level routine for getting a message
 *
 ***************************************************************/
mpsr_Status_t mpsr_GetMsg(MP_Link_pt link, leftv &lv)
{
  mpsr_sleftv mlv, mlv1;
  mpsr_Status_t status = mpsr_Success;
  mlv.lv = NULL;
  mlv.r = NULL;
  mlv1.lv = NULL;
  mlv1.r = NULL;

  status = (MP_InitMsg(link) == MP_Success ? mpsr_Success : mpsr_MP_Failure);

  if (status == mpsr_Success && ! MP_TestEofMsg(link))
    status = mpsr_GetLeftv(link, &mlv, 0);
  else
  {
    lv = mpsr_InitLeftv(NONE, NULL);
    return status;
  }

  // handle more than one leftv (ie. chains of leftv's)
  while (status == mpsr_Success && ! MP_TestEofMsg(link))
  {
    // Get next leftv
    status = mpsr_GetLeftv(link, &mlv1, 0);
    if (status == mpsr_Success)
      status = mpsr_MergeLeftv(&mlv, &mlv1);
  }

  if (status == mpsr_Success)
  {
    // Now mlv is our leftv -- check whether r has an ordering set
    if (mlv.r != NULL && mlv.r->order[0] == ringorder_unspec)
    {
        ring r = rCopy(mlv.r);
        r->order[0] = ringorder_dp;
        mpsr_rSetOrdSgn(r);
        mpsr_MapLeftv(mlv.lv, mlv.r, r);
        rKill(mlv.r);
        mlv.r = r;
    }

    mpsr_SetCurrRingHdl(&mlv);

    lv = mlv.lv;
  }
  else lv = mpsr_InitLeftv(NONE, NULL);

  return status;
}

/***************************************************************
 *
 * Top-level routine for getting a mpsr_leftv
 *
 ***************************************************************/

// if quote > 0, then identifiers are not tried to be resolved, or
// converted into a ring variable
mpsr_Status_t mpsr_GetLeftv(MP_Link_pt link, mpsr_leftv mlv, short quote)
{
  MPT_Node_pt node = NULL;
  MP_NodeType_t type;
  mlv->r = NULL;
  mlv->lv = NULL;

  mpt_failr(MPT_GetNode(link, &node));

  type = node->type;

  if (MP_IsFixedIntegerType(type))
    failr(mpsr_GetIntLeftv(node, mlv));
  else if (MP_IsIdType(type))
    failr(mpsr_GetIdentifierLeftv(node, mlv, quote));
  else
  {
    switch (node->type)
    {
        case MP_ApIntType:
          failr(mpsr_GetApIntLeftv(node, mlv));
          break;

        case MP_StringType:
          failr(mpsr_GetStringLeftv(node, mlv));
          break;

        case MP_Real32Type:
          failr(mpsr_GetReal32Leftv(node, mlv));
          break;

        case MP_CommonOperatorType:
          failr(mpsr_GetCommonOperatorLeftv(link, node, mlv, quote));
          break;

        case MP_OperatorType:
          failr(mpsr_GetOperatorLeftv(link, node, mlv, quote));
          break;

        default:
          MPT_DeleteNode(node);
          return mpsr_SetError(mpsr_UnknownMPNodeType);
    }
  }

  // everything was ok
  MPT_DeleteNode(node);
  return mpsr_Success;
}

/***************************************************************
 *
 * mpsr_Get*Leftv
 *
 ***************************************************************/
mpsr_Status_t mpsr_GetCommonOperatorLeftv(MP_Link_pt link,
                                          MPT_Node_pt node,
                                          mpsr_leftv mlv,
                                          short quote)
{
  mpsr_assume(node->type == MP_CommonOperatorType);
  BOOLEAN IsUnOrdered;

  // Check for Singular data types
  // IntVec
  if (IsIntVecNode(node))
    return GetIntVecLeftv(link, node, mlv);
  // IntMat
  else if (IsIntMatNode(node))
    return GetIntMatLeftv(link, node, mlv);
  // Ring
  else if (IsRingNode(node, mlv->r, IsUnOrdered))
    return GetRingLeftv(link, node, mlv);
  // Poly
  else if (IsPolyNode(node, mlv->r, IsUnOrdered))
    return GetPolyLeftv(link, node, mlv, IsUnOrdered);
  // PolyVector
  else if (IsPolyVectorNode(node, mlv->r, IsUnOrdered))
    return GetPolyVectorLeftv(link, node, mlv, IsUnOrdered);
  // Ideal
  else if (IsIdealNode(node, mlv->r, IsUnOrdered))
    return GetIdealLeftv(link, node, mlv, IsUnOrdered);
  // Module
  else if (IsModuleNode(node, mlv->r, IsUnOrdered))
    return GetModuleLeftv(link, node, mlv, IsUnOrdered);
  // Matrix
  else if (IsMatrixNode(node, mlv->r, IsUnOrdered))
    return GetMatrixLeftv(link, node, mlv, IsUnOrdered);
  else if (IsQuitNode(node))
    return GetQuitLeftv(mlv);
  // Map
  else
    // now it should be a command (which handles Proc, Map and List
    // seperately)
    return GetCopCommandLeftv(link, node, mlv, quote);
}

mpsr_Status_t mpsr_GetOperatorLeftv(MP_Link_pt link,
                                  MPT_Node_pt node,
                                  mpsr_leftv mlv,
                                  short quote)
{
  mpsr_assume(node->type == MP_OperatorType);
  MP_NumChild_t nc = node->numchild, i;
  mpsr_sleftv smlv1, *mlv1 = &smlv1;


  if (MPT_ProtoAnnotValue(node) != NULL)
    return mpsr_SetError(mpsr_CanNotHandlePrototype);

  if (nc > 0)
  {
    failr(mpsr_GetLeftv(link, mlv, quote));
    for (i=1; i<nc; i++)
    {
      failr(mpsr_GetLeftv(link, mlv1, quote));
      failr(mpsr_MergeLeftv(mlv, mlv1));
    }
  }

  command cmd = (command) omAlloc0Bin(sip_command_bin);
  cmd->op = PROC_CMD;
  cmd->arg1.rtyp = STRING_CMD;
  cmd->arg1.data = (void *) omStrDup(MP_STRING_T(node->nvalue));

  if (node->numchild > 0)
  {
    cmd->argc = 2;
    memcpy(&(cmd->arg2), mlv->lv, sizeof(sleftv));
    omFreeBin(mlv->lv, sleftv_bin);
  }
  else cmd->argc = 1;

  mlv->lv = mpsr_InitLeftv(COMMAND, (void *) cmd);
  return mpsr_Success;
}

/***************************************************************
 *
 * Get*Leftv routines
 *
 ***************************************************************/
//
// Get*Leftv routines
//
static mpsr_Status_t GetIntVecLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv)
{
  intvec *iv = new intvec(node->numchild);
  int *v = iv->ivGetVec();

  mp_failr(IMP_GetSint32Vector(link, &v, node->numchild));
  mlv->lv = mpsr_InitLeftv(INTVEC_CMD, (void *) iv);
  return mpsr_Success;
}

static mpsr_Status_t GetIntMatLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv)
{
  intvec *iv;
  int row = node->numchild, col = 1, *v;
  MPT_Annot_pt annot = MPT_Annot(node, MP_MatrixDict,
                                 MP_AnnotMatrixDimension);
  if (annot != NULL &&
      annot->value != NULL &&
      annot->value->node->numchild == 2 &&
      NodeCheck(annot->value->node, MP_CommonOperatorType, MP_BasicDict,
                MP_CopBasicList))
  {
    MPT_Tree_pt *tarray = (MPT_Tree_pt *) annot->value->args;
    if (tarray[0]->node->type == MP_Sint32Type &&
        tarray[1]->node->type == MP_Sint32Type)
    {
      row = MP_SINT32_T(tarray[0]->node->nvalue);
      col = MP_SINT32_T(tarray[1]->node->nvalue);
    }
  }

  iv = new intvec(row, col, 0);
  v = iv->ivGetVec();
  mp_failr(IMP_GetSint32Vector(link, &v, node->numchild));
  mlv->lv = mpsr_InitLeftv(INTMAT_CMD, (void *) iv);
  return mpsr_Success;
}

static mpsr_Status_t GetRingLeftv(MP_Link_pt link, MPT_Node_pt node,
                                mpsr_leftv mlv)
{
  mpsr_assume(mlv->r != NULL);

  if (node->numchild != 0) mpt_failr(MPT_SkipArgs(link, node));

  mlv->lv = mpsr_InitLeftv(((mlv->r->qideal != NULL) ? (short) QRING_CMD :
                            (short) RING_CMD),
                           (void *) mlv->r);
  mlv->r = NULL;
  return mpsr_Success;
}

static mpsr_Status_t GetPolyLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv, BOOLEAN IsUnOrdered)
{
  poly p;

  mpsr_assume(mlv->r != NULL);

  failr(mpsr_GetPoly(link, p, node->numchild, mlv->r));
  if (IsUnOrdered) p = pSort(p);
  pTest(p);
  mlv->lv = mpsr_InitLeftv(POLY_CMD, (void *) p);
  return mpsr_Success;
}

static mpsr_Status_t GetPolyVectorLeftv(MP_Link_pt link, MPT_Node_pt node,
                                      mpsr_leftv mlv, BOOLEAN IsUnOrdered)
{
  poly p;

  mpsr_assume(mlv->r != NULL);

  failr(mpsr_GetPolyVector(link, p, node->numchild, mlv->r));
  if (IsUnOrdered) p = pSort(p);
  pTest(p);
  mlv->lv = mpsr_InitLeftv(VECTOR_CMD, (void *) p);
  return mpsr_Success;
}

static mpsr_Status_t GetIdealLeftv(MP_Link_pt link, MPT_Node_pt node,
                                 mpsr_leftv mlv, BOOLEAN IsUnOrdered)
{
  MP_NumChild_t nc = node->numchild, i;
  ring r = mlv->r;
  MP_Uint32_t nmon;

  mpsr_assume(r != NULL);
  ideal id = idInit(nc,1);
  for (i=0; i<nc; i++)
  {
    mp_failr(IMP_GetUint32(link, &nmon));
    failr(mpsr_GetPoly(link, id->m[i], nmon, r));
    if (IsUnOrdered) id->m[i] = pSort(id->m[i]);
  }
  idTest(id);
  mlv->lv = mpsr_InitLeftv(IDEAL_CMD, (void *) id);
  return mpsr_Success;
}

static mpsr_Status_t GetModuleLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv, BOOLEAN IsUnOrdered)
{
  MP_NumChild_t nc = node->numchild, i;
  ring r = mlv->r;
  MP_Uint32_t nmon, rank = 1;
  MPT_Annot_pt annot = MPT_Annot(node, MP_PolyDict,
                                     MP_AnnotPolyModuleRank);
  if (annot != NULL &&
      annot->value != NULL &&
      annot->value->node->type == MP_Uint32Type
      )
    rank = MP_UINT32_T(annot->value->node->nvalue);

  mpsr_assume(r != NULL);
  ideal id = idInit(nc,rank);
  for (i=0; i<nc; i++)
  {
    mp_failr(IMP_GetUint32(link, &nmon));
    failr(mpsr_GetPolyVector(link, id->m[i], nmon, r));
    if (IsUnOrdered) id->m[i] = pSort(id->m[i]);
  }
  if (rank == 1)
    id->rank = idRankFreeModule(id);
  idTest(id);
  mlv->lv = mpsr_InitLeftv(MODUL_CMD, (void *) id);
  return mpsr_Success;
}

static mpsr_Status_t GetMatrixLeftv(MP_Link_pt link, MPT_Node_pt node,
                                  mpsr_leftv mlv, BOOLEAN IsUnOrdered)
{
  MP_NumChild_t nc = node->numchild, row = nc, col = 1, i;
  matrix mp;
  MP_Uint32_t nmon;
  MPT_Annot_pt annot = MPT_Annot(node, MP_MatrixDict,
                                     MP_AnnotMatrixDimension);
  if (annot != NULL &&
      annot->value != NULL &&
      annot->value->node->numchild == 2 &&
      NodeCheck(annot->value->node, MP_CommonOperatorType, MP_BasicDict,
               MP_CopBasicList))
  {
    MPT_Tree_pt *tarray = (MPT_Tree_pt *) annot->value->args;
    if (tarray[0]->node->type == MP_Sint32Type &&
        tarray[1]->node->type == MP_Sint32Type)
    {
      row = MP_SINT32_T(tarray[0]->node->nvalue);
      col = MP_SINT32_T(tarray[1]->node->nvalue);
    }
  }

  mpsr_assume(mlv->r != NULL);
  mp = mpNew(row, col);
  for (i=0; i<nc; i++)
  {
    mp_failr(IMP_GetUint32(link, &nmon));
    failr(mpsr_GetPoly(link, mp->m[i], nmon, mlv->r));
    if (IsUnOrdered) mp->m[i] = pSort(mp->m[i]);
  }

  mlv->lv = mpsr_InitLeftv(MATRIX_CMD, (void *) mp);
  return mpsr_Success;
}

static mpsr_Status_t GetPackageLeftv(MP_Link_pt link, MPT_Node_pt node,
                                     mpsr_leftv mlv)
{
  package pack = (package) omAlloc0Bin(sip_package_bin);

  pack->language = LANG_NONE;

  if (node->numchild > 0)
  {
    failr(mpsr_GetLeftv(link, mlv, 0));
    if (mlv->lv->rtyp != STRING_CMD)
      return mpsr_SetError(mpsr_WrongArgumentType);
    pack->libname = (char*) mlv->lv->data;
    mlv->lv->rtyp = PACKAGE_CMD;
  }
  else
    mlv->lv = mpsr_InitLeftv(PACKAGE_CMD, NULL);

  mlv->lv->data = (void*) pack;

  MPT_Annot_pt annot = MPT_Annot(node, MP_SingularDict,
                                 MP_AnnotSingularPackageType);
  if (annot != NULL && annot->value != NULL && annot->value->node)
  {
    MPT_Node_pt node = annot->value->node;
    if (node->type == MP_Uint8Type && MP_UINT8_T(node->nvalue) < LANG_MAX)
      pack->language =  (language_defs) MP_UINT8_T(node->nvalue);
  }
  return mpsr_Success;
}


static mpsr_Status_t GetMapLeftv(MP_Link_pt link, MPT_Node_pt node,
                               mpsr_leftv mlv)
{
  mpsr_sleftv smlv1, smlv2, *mlv1 = &smlv1, *mlv2 = &smlv2;

  if (node->numchild != 3)
    return mpsr_SetError(mpsr_WrongNumofArgs);

  failr(mpsr_GetLeftv(link, mlv, 0));
  failr(mpsr_GetLeftv(link, mlv1, 0));
  failr(mpsr_GetLeftv(link, mlv2, 0));

  if (mlv->lv->rtyp != RING_CMD ||
      mlv1->lv->rtyp != STRING_CMD ||
      mlv2->lv->rtyp != IDEAL_CMD)
    return mpsr_SetError(mpsr_WrongArgumentType);

  ring r = (ring) mlv->lv->data, r2;
  char *name = (char *) mlv1->lv->data;
  ideal id = (ideal) mlv2->lv->data;

  idhdl h = mpsr_FindIdhdl(name, r2);
  if (h == NULL || IDTYP(h) != RING_CMD || ! rEqual(IDRING(h), r))
  {
    h = mpsr_InitIdhdl(RING_CMD, r, name);
    h->next = IDROOT;
    IDROOT = h;
  }

  map m = (map) omAlloc0Bin(sip_sideal_bin);
  m->preimage = omStrDup(name);
  m->m = id->m;
  m->nrows = id->nrows;
  m->ncols = id->ncols;

  omFreeBin(mlv->lv, sleftv_bin);

  omFree(mlv1->lv->data);
  omFreeBin(mlv1->lv, sleftv_bin);

  omFreeBin(id, sip_sideal_bin);
  omFreeBin(mlv2->lv, sleftv_bin);

  mlv->r = mlv2->r;
  mlv->lv = mpsr_InitLeftv(MAP_CMD, (void *) m);

  return mpsr_Success;
}


static mpsr_Status_t GetCopCommandLeftv(MP_Link_pt link, MPT_Node_pt node,
                                        mpsr_leftv mlv, short quote)
{
  short tok;
  MP_NumChild_t nc = node->numchild, i;
  mpsr_sleftv smlv1, *mlv1 = &smlv1;
  MPT_Tree_pt typespec;


  failr(mpsr_mp2tok(node->dict, MP_COMMON_T(node->nvalue), &tok));

  if ((typespec = MPT_ProtoAnnotValue(node)) &&
      MPT_IsTrueProtoTypeSpec(typespec))
    return mpsr_SetError(mpsr_CanNotHandlePrototype);

  if (tok == MAP_CMD) return GetMapLeftv(link, node, mlv);
  if (tok == PACKAGE_CMD) return GetPackageLeftv(link, node, mlv);
  if (tok == COLONCOLON) quote++;

  if (nc > 0)
  {
    if (tok == '=') failr(mpsr_GetLeftv(link, mlv, quote + 1));
    else failr(mpsr_GetLeftv(link, mlv, quote));
  }

  for (i=1; i<nc; i++)
  {
    failr(mpsr_GetLeftv(link, mlv1, quote));
    failr(mpsr_MergeLeftv(mlv, mlv1));
  }

  command cmd = (command) omAlloc0Bin(sip_command_bin);
  cmd->op = tok;
  cmd->argc = nc;

  // check that associative operators are binary
  if (nc > 2 && (tok == '+' || tok == '*'))
  {
    leftv lv = mlv->lv, lv2;
    command c = cmd, c2;

    for (i=2; i<nc; i++)
    {
      c->op = tok;
      c->argc = 2;
      memcpy(&(c->arg1), lv, sizeof(sleftv));
      c->arg1.next = NULL;

      c2 = (command) omAlloc0Bin(sip_command_bin);
      c->arg2.data = (void *) c2;
      c->arg2.rtyp = COMMAND;
      c = c2;
      lv2 = lv->next;
      omFreeBin(lv, sleftv_bin);
      lv = lv2;
    }
    c->op = tok;
    c->argc = 2;
    memcpy(&(c->arg1), lv, sizeof(sleftv));
    c->arg1.next = NULL;
    memcpy(&(c->arg2), lv->next, sizeof(sleftv));
    omFreeBin(lv->next, sleftv_bin);
    omFreeBin(lv, sleftv_bin);
  }
  else if (nc >= 1)
  {
    memcpy(&(cmd->arg1), mlv->lv, sizeof(sleftv));
    if (nc <= 3)
    {
      (cmd->arg1).next = NULL;
      if (nc >= 2)
      {
        memcpy(&(cmd->arg2), mlv->lv->next, sizeof(sleftv));
        (cmd->arg2).next = NULL;

        if (nc == 3)
        {
          memcpy(&(cmd->arg3), mlv->lv->next->next, sizeof(sleftv));
          omFreeBin(mlv->lv->next->next, sleftv_bin);
        }
        omFreeBin(mlv->lv->next, sleftv_bin);
      }
    }
    omFreeBin(mlv->lv, sleftv_bin);
  }

  // Now we perform some tree manipulations
  if (nc == 0 && tok == LIST_CMD)
    // Here we work around a Singular bug: It can not handle lists of 0 args
    // so we construct them explicitely
  {
    lists l = (lists) omAllocBin(slists_bin);
    l->Init(0);
    mlv->lv = mpsr_InitLeftv(LIST_CMD, (void *) l);
    return mpsr_Success;
  }
  mlv->lv = mpsr_InitLeftv(COMMAND, (void *) cmd);
  return mpsr_Success;
}


/***************************************************************
 *
 * The routine for Getting External Data
 *
 ***************************************************************/

// this is all the data we want to have in directly when MPT_GetTree
// is called, i.e. when the value of annots is read
MPT_Status_t mpsr_GetExternalData(MP_Link_pt link,
                                  MPT_Arg_t *odata,
                                  MPT_Node_pt node)
{
  *odata = NULL;

  if (node->type == MP_CommonOperatorType)
  {
    mpsr_sleftv mlv;
    mpsr_Status_t status;
    BOOLEAN IsUnOrdered;

    // we would like to get polys and ideals directly
    if (IsPolyNode(node, mlv.r, IsUnOrdered))
      status = GetPolyLeftv(link, node, &mlv, IsUnOrdered);
    // Ideal
    else if (IsIdealNode(node, mlv.r, IsUnOrdered))
      status = GetIdealLeftv(link, node, &mlv, IsUnOrdered);
    else
      return MPT_NotExternalData;

    if (status == mpsr_Success)
    {
      mpsr_leftv mmlv = (mpsr_leftv) omAlloc0Bin(mpsr_sleftv_bin);
      memcpy(mmlv, &mlv, sizeof(mpsr_sleftv));
      *odata = (MPT_ExternalData_t) mmlv;
      return MPT_Success;
    }
    else
      return MPT_Failure;
  }
  else
    return MPT_NotExternalData;
}


/***************************************************************
 *
 * A routine which gets the previous dump of Singular
 *
 ***************************************************************/

mpsr_Status_t mpsr_GetDump(MP_Link_pt link)
{
  mpsr_sleftv mlv;
  mpsr_Status_t status = mpsr_Success;

  status = (MP_InitMsg(link) == MP_Success ? mpsr_Success : mpsr_MP_Failure);
  while ((status == mpsr_Success) && (! MP_TestEofMsg(link)))
  {
    memset(&mlv,0,sizeof(mlv));
    status=mpsr_GetLeftv(link, &mlv, 0);

    if (status == mpsr_Success)
    {
#ifdef MPSR_DEBUG_DUMP
      command cmd = (command) mlv.lv->data;
      fprintf(stdout, "Dump got %s \n", cmd->arg1.name);
      fflush(stdout);
#endif
      mpsr_SetCurrRingHdl(&mlv);
      if (mlv.lv != NULL)
      {
        mlv.lv->Eval();
        mlv.lv->CleanUp();
        omFreeBin(mlv.lv, sleftv_bin);
      }
    }
    else
      mpsr_PrintError(status);
  }
  return status;
}
#endif
