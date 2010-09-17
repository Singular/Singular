/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_Put.cc
 * Purpose:    main put routines for MP connection to Singular
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *  o 1/97 obachman
 *    Updated putting routines to MP and MPP v1.1
 *
 ***************************************************************/
#include <kernel/mod2.h>

#ifdef HAVE_MPSR

#include <kernel/structs.h>
#include <Singular/mpsr_Put.h>
#include <Singular/mpsr_Tok.h>
#include <kernel/intvec.h>
#include <Singular/lists.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>

/***************************************************************
 *
 * There are 4 different layers on which things are put:
 * 1.) Singular Top-level Data (chains of leftv's) == MP message
 * 2.) Single leftv's
 * 3.) Plain Singular data (different Singular data types)
 * 4.) Primitive polynomial Data
 *
 ***************************************************************/

/***************************************************************
 * 1.) Routines for Top-level Data
 *
 ***************************************************************/
// this puts everything it finds in a leftv on the MP-link in form of
// a message; Each leftv is sent as one MP Tree. The leftvs in a chain
// of leftv's (i.e. for v->next != NULL) are sent as separate MP Trees
// within the same MP message
mpsr_Status_t mpsr_PutMsg(MP_Link_pt l, leftv v)
{
  mpsr_Status_t status = mpsr_Success;

  MP_ResetLink(l);
  while (v != NULL && status == mpsr_Success)
  {
    status = mpsr_PutLeftv(l, v, currRing);
    v = v->next;
  }
  MP_EndMsg(l);

  return status;
}


/***************************************************************
 * Second-level data routines
 * All the mpsr_Put*Leftv functions are defined as inline's in the header
 *
 ***************************************************************/

// This already depends on the ring
mpsr_Status_t mpsr_PutLeftv(MP_Link_pt link, leftv v, ring cring)
{
  int type = v->Typ();

  switch(type)
  {
    // first, all the ring-independent types
      case INT_CMD:
        return mpsr_PutIntLeftv(link,v);

      case INTVEC_CMD:
        return mpsr_PutIntVecLeftv(link, v);

      case INTMAT_CMD:
        return mpsr_PutIntMatLeftv(link, v);

      case STRING_CMD:
        return mpsr_PutStringLeftv(link, v);

      case RING_CMD:
        return mpsr_PutRingLeftv(link, v);

      case QRING_CMD:
        return mpsr_PutQRingLeftv(link, v);

      case PROC_CMD:
        return mpsr_PutProcLeftv(link, v);

      case DEF_CMD:
        return mpsr_PutDefLeftv(link, v);

        // now potentially ring-dependent types
      case LIST_CMD:
        return mpsr_PutListLeftv(link,v, cring);

      case COMMAND:
        return mpsr_PutCommandLeftv(link,v, cring);

      case NUMBER_CMD:
        return mpsr_PutNumberLeftv(link,v, cring);

      case POLY_CMD:
        return mpsr_PutPolyLeftv(link, v, cring);

      case IDEAL_CMD:
        return mpsr_PutIdealLeftv(link, v, cring);

      case VECTOR_CMD:
        return mpsr_PutVectorLeftv(link, v, cring);

      case MODUL_CMD:
        return mpsr_PutModuleLeftv(link, v, cring);

      case MATRIX_CMD:
        return mpsr_PutMatrixLeftv(link,v, cring);

      case MAP_CMD:
        return mpsr_PutMapLeftv(link, v, cring);

      case PACKAGE_CMD:
        return mpsr_PutPackageLeftv(link, v);

      case NONE:
        return mpsr_Success;

      default:
        return mpsr_SetError(mpsr_UnknownLeftvType);
  }
}

/***************************************************************
 * the Third-level data routines which are ring-independent
 *
 ***************************************************************/
mpsr_Status_t mpsr_PutIntVec(MP_Link_pt link, intvec *iv)
{
  int length = iv->length();

  // Put the Vector Operator
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_MatrixDict,
                                      MP_CopMatrixDenseVector,
                                      1,
                                      length));
  // Prototype Annot
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  // Together with the CommonMetaTypePacket specifying that each element of
  // the vector is an Sint32
  mp_failr(MP_PutCommonMetaTypePacket(link,
                                      MP_ProtoDict,
                                      MP_CmtProtoIMP_Sint32,
                                      0));

  // Now we finally put the data
  mp_return(IMP_PutSint32Vector(link, (MP_Sint32_t *) iv->ivGetVec(),
                                length));
}

mpsr_Status_t mpsr_PutIntMat(MP_Link_pt link, intvec *iv)
{
  int r = iv->rows(), c = iv->cols(), length = r*c;

  // First, we put the Matrix operator
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_MatrixDict,
                                      MP_CopMatrixDenseMatrix,
                                      2,
                                      length));
  // Put the two annotations
  // First, the prototype annot
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  mp_failr(MP_PutCommonMetaTypePacket(link,
                                  MP_ProtoDict,
                                  MP_CmtProtoIMP_Sint32,
                                  0));
  // And second, the dimension annot
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_MatrixDict,
                                  MP_AnnotMatrixDimension,
                                  MP_AnnotReqValNode));
  // which specifies the dimesnion of the matrix
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_BasicDict,
                                      MP_CopBasicList,
                                      0,
                                      2));
  mp_failr(MP_PutSint32Packet(link, (MP_Sint32_t) r, 0));
  mp_failr(MP_PutSint32Packet(link, (MP_Sint32_t) c, 0));

  // And finally, we put the elments
  mp_return(IMP_PutSint32Vector(link, (MP_Sint32_t *) iv->ivGetVec(),
                                length));
}

mpsr_Status_t mpsr_PutRing(MP_Link_pt link, ring cring)
{
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_PolyDict,
                                      MP_CopPolyRing,
                                      mpsr_GetNumOfRingAnnots(cring, 1),
                                      0));
  return mpsr_PutRingAnnots(link, cring, 1);
}

mpsr_Status_t mpsr_PutProc(MP_Link_pt link, const char* pname, procinfov proc)
{
  MP_DictTag_t dict;
  MP_Common_t  cop;
  char *iiGetLibProcBuffer(procinfov pi, int part=1);

  failr(mpsr_tok2mp('=', &dict, &cop));

  // A Singular- procedure is sent as a cop with the string as arg
  mp_failr(MP_PutCommonOperatorPacket(link,
                                        dict,
                                        cop,
                                        0,
                                        2));
  if (pname==NULL)
    pname=proc->procname;

  mp_failr(MP_PutIdentifierPacket(link, MP_SingularDict, pname,1));
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_SingularDict,
                                  MP_AnnotSingularProcDef,
                                  0));
  if( proc->language == LANG_SINGULAR) {
    if (proc->data.s.body == NULL)
      iiGetLibProcBuffer(proc);
    mp_return(MP_PutStringPacket(link, proc->data.s.body, 0));
  }
  else
    mp_return(MP_PutStringPacket(link, "", 0));
  mp_return(MP_Success);
}


/***************************************************************
 * the Third-level data routines which are ring-dependent
 *
 ***************************************************************/

static inline mpsr_Status_t PutLeftvChain(MP_Link_pt link, leftv lv, ring r)
{
  while (lv != NULL)
  {
    failr(mpsr_PutLeftv(link, lv, r));
    lv = lv->next;
  }
  return mpsr_Success;
}

mpsr_Status_t mpsr_PutList(MP_Link_pt link, lists l, ring cring)
{
  int i, nl = l->nr + 1;

  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_BasicDict,
                                      MP_CopBasicList,
                                      0,
                                      nl));
  for (i=0; i<nl; i++)
    failr(mpsr_PutLeftv(link, &(l->m[i]), cring));

  return mpsr_Success;
}

mpsr_Status_t mpsr_PutCopCommand(MP_Link_pt link,  command cmd, ring cring)
{
  MP_Common_t cop;
  MP_DictTag_t dict;
  MP_NumChild_t nc = cmd->argc;
  leftv l;

  // First, we get the command cop -- at the moment, everything should be
  // a MP cop
  failr(mpsr_tok2mp(cmd->op, &dict, &cop));

  // and put the common operator
  mp_failr(MP_PutCommonOperatorPacket(link, dict, cop, 0, nc));

  // now we Put the arguments
  if (nc > 0)
  {
    if (nc <= 3)
    {
      failr(mpsr_PutLeftv(link, &(cmd->arg1), cring));
      if (nc >1)
      {
        failr(mpsr_PutLeftv(link, &(cmd->arg2), cring));
        if (nc == 3) return mpsr_PutLeftv(link, &(cmd->arg3), cring);
        else return mpsr_Success;
      }
      else return mpsr_Success;
    }
    else
      return PutLeftvChain(link, &(cmd->arg1), cring);
  }
  return mpsr_Success;
}

mpsr_Status_t mpsr_PutOpCommand(MP_Link_pt link,  command cmd, ring cring)
{
  mp_failr(MP_PutOperatorPacket(link,
                                MP_SingularDict,
                                (char *) (cmd->arg1.Data()),
                                0,
                                (cmd->argc <= 1 ? 0 :(cmd->arg2).listLength())));
  if (cmd->argc > 1)
    return PutLeftvChain(link, &(cmd->arg2), cring);
  else
    return mpsr_Success;
}

// Numbers are put as polys with one monomial and all exponents zero
mpsr_Status_t mpsr_PutNumber(MP_Link_pt link,  number n, ring cring)
{
  ring rr = NULL;
  poly p = NULL;
  mpsr_Status_t r;

  if (currRing != cring)
  {
    rr = currRing;
    mpsr_SetCurrRing(cring, TRUE);
  }

  if (!nIsZero(n))
  {
    p = pOne();
    pSetCoeff(p, nCopy(n));
  }
  r = mpsr_PutPoly(link, p, cring);
  pDelete(&p);

  if (rr != NULL) mpsr_SetCurrRing(rr, TRUE);

  return r;
}

mpsr_Status_t mpsr_PutPoly(MP_Link_pt link, poly p, ring cring)
{
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_PolyDict,
                                      MP_CopPolyDenseDistPoly,
                                      mpsr_GetNumOfRingAnnots(cring, 0),
                                      pLength(p)));
  failr(mpsr_PutRingAnnots(link, cring, 0));
  return mpsr_PutPolyData(link, p, cring);
}


mpsr_Status_t mpsr_PutPolyVector(MP_Link_pt link, poly p, ring cring)
{
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_PolyDict,
                                      MP_CopPolyDenseDistPoly,
                                      mpsr_GetNumOfRingAnnots(cring,1),
                                      pLength(p)));
  failr(mpsr_PutRingAnnots(link, cring, 1));
  return mpsr_PutPolyVectorData(link, p, cring);
}


mpsr_Status_t mpsr_PutIdeal(MP_Link_pt link, ideal id, ring cring)
{
  int i, idn = IDELEMS(id);

  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_PolyDict,
                                      MP_CopPolyIdeal,
                                      1,
                                      idn));
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  mp_failr(MP_PutCommonMetaOperatorPacket(link,
                                          MP_PolyDict,
                                          MP_CopPolyDenseDistPoly,
                                          mpsr_GetNumOfRingAnnots(cring, 0),
                                          0));
  failr(mpsr_PutRingAnnots(link, cring, 0));

  for (i=0; i < idn; i++)
  {
    IMP_PutUint32(link, pLength(id->m[i]));
    failr(mpsr_PutPolyData(link, id->m[i], cring));
  }
  return mpsr_Success;
}

mpsr_Status_t mpsr_PutModule(MP_Link_pt link, ideal id, ring cring)
{
  int i, idn = IDELEMS(id);

  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_PolyDict,
                                      MP_CopPolyModule,
                                      2,
                                      idn));
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  mp_failr(MP_PutCommonMetaOperatorPacket(link,
                                          MP_PolyDict,
                                          MP_CopPolyDenseDistPoly,
                                          mpsr_GetNumOfRingAnnots(cring, 1),
                                          0));
  failr(mpsr_PutRingAnnots(link, cring, 1));

  mp_failr(MP_PutAnnotationPacket(link,
                                    MP_PolyDict,
                                    MP_AnnotPolyModuleRank,
                                    MP_AnnotValuated));
  mp_failr(MP_PutUint32Packet(link, id->rank, 0));

  for (i=0; i < idn; i++)
  {
    IMP_PutUint32(link, pLength(id->m[i]));
    failr(mpsr_PutPolyVectorData(link, id->m[i], cring));
  }
  return mpsr_Success;
}

mpsr_Status_t mpsr_PutMatrix(MP_Link_pt link, ideal id, ring cring)
{
  int nrows = id->nrows, ncols = id->ncols;
  MP_Uint32_t n = nrows*ncols, i;

  // First, we put the Matrix operator
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_MatrixDict,
                                      MP_CopMatrixDenseMatrix,
                                      2,
                                      n));
  // Put the two annotations
  // First, the prototype annot
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  mp_failr(MP_PutCommonMetaOperatorPacket(link,
                                          MP_PolyDict,
                                          MP_CopPolyDenseDistPoly,
                                          mpsr_GetNumOfRingAnnots(cring, 0),
                                          0));
  failr(mpsr_PutRingAnnots(link, cring, 0));

  // second, the matrix dim annot
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_MatrixDict,
                                  MP_AnnotMatrixDimension,
                                  MP_AnnotReqValNode));
  // which specifies the dimesnion of the matrix
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_BasicDict,
                                      MP_CopBasicList,
                                      0, 2));
  mp_failr(MP_PutSint32Packet(link, (MP_Sint32_t) nrows, 0));
  mp_failr(MP_PutSint32Packet(link, (MP_Sint32_t) ncols, 0));

  // And finally, we put the elments
  for (i=0; i < n; i++)
  {
    IMP_PutUint32(link, pLength(id->m[i]));
    failr(mpsr_PutPolyData(link, id->m[i], cring));
  }
  return mpsr_Success;
}


// We communicate a map as an operator having three arguments: A ring,
// its name and an ideal
mpsr_Status_t mpsr_PutMap(MP_Link_pt link, map m, ring cring)
{
  MP_Uint32_t i, idn = IDELEMS((ideal) m);
  MP_DictTag_t dict;
  MP_Common_t cop;

  failr(mpsr_tok2mp(MAP_CMD, &dict, &cop));

  mp_failr(MP_PutCommonOperatorPacket(link,
                                      dict,
                                      cop,
                                      0,
                                      3));
  // First, is the ring
  failr(mpsr_PutRingLeftv(link, (leftv) IDROOT->get(m->preimage, 1)));

  // Second, the name of the ring
  mp_failr(MP_PutStringPacket(link, m->preimage,0));

  // and third, the ideal --
  // supposing that we can cast a map to an ideal
  return mpsr_PutIdeal(link, (ideal) m, cring);
}

mpsr_Status_t mpsr_PutPackage(MP_Link_pt link, package pack)
{
  MP_DictTag_t dict;
  MP_Common_t  cop;

  failr(mpsr_tok2mp(PACKAGE_CMD, &dict, &cop));

  // A Singular- procedure is sent as a cop with the string as arg
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      dict,
                                      cop,
                                      1,
                                      (pack->language != LANG_NONE &&
                                       pack->language != LANG_TOP ? 1 : 0)));
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_SingularDict,
                                  MP_AnnotSingularPackageType,
                                  MP_AnnotValuated));
  mp_failr(MP_PutUint8Packet(link, (unsigned char) pack->language, 0));
  if (pack->language != LANG_NONE && pack->language != LANG_TOP)
  {
    assume(pack->libname != NULL);
    mp_failr(MP_PutStringPacket(link, pack->libname, 0));
  }
  mp_return(MP_Success);
}

/***************************************************************
 *
 * A routine which dumps the content of Singular to a file
 *
 ***************************************************************/
mpsr_Status_t mpsr_PutDump(MP_Link_pt link)
{
  idhdl h = IDROOT, h2 = NULL, rh = currRingHdl;
  ring r;
  sip_command cmd;
  leftv lv;

  mpsr_ClearError();
  memset(&(cmd), 0, sizeof(sip_command));
  cmd.argc = 2;
  cmd.op = '=';
  cmd.arg1.rtyp = DEF_CMD;
  lv = mpsr_InitLeftv(COMMAND, (void *) &cmd);

  MP_ResetLink(link);
  while (h != NULL && h2 == NULL)
  {

    if (IDTYP(h) == PROC_CMD)
    {
      failr(mpsr_PutLeftv(link, (leftv) h, NULL));
#ifdef MPSR_DEBUG_DUMP
      Print("Dumped Proc %s\n", IDID(h));
#endif
    }
    // do not dump LIB string and Links and Top PACKAGE
    else if (!(IDTYP(h) == STRING_CMD && strcmp("LIB", IDID(h)) == 0) &&
             IDTYP(h) != LINK_CMD &&
             ! (IDTYP(h) == PACKAGE_CMD && strcmp(IDID(h), "Top") == 0))
    {
      cmd.arg1.name = IDID(h);
      cmd.arg2.data=IDDATA(h);
      cmd.arg2.flag=h->flag;
      cmd.arg2.attribute=h->attribute;
      cmd.arg2.rtyp=h->typ;
      if (mpsr_PutLeftv(link, lv, r) != mpsr_Success) break;

#ifdef MPSR_DEBUG_DUMP
      Print("Dumped %s\n", IDID(h));
#endif
      if (IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD 
#if 0
      || (IDTYP(h) == PACKAGE_CMD && strcmp(IDID(h), "Top") != 0)
#endif
      )
      {
        // we don't really need to do that, it's only for convenience
        // for putting numbers
#if 0
        if (IDTYP(h) == PACKAGE_CMD)
        {
          namespaceroot->push(IDPACKAGE(h), IDID(h));
          h2 = IDPACKAGE(h)->idroot;
        }
        else
#endif
        {
          rSetHdl(h);
          r = IDRING(h);
          h2 = r->idroot;
        }
        while (h2 != NULL)
        {
          cmd.arg1.name = IDID(h2);
          cmd.arg2.data=IDDATA(h2);
          cmd.arg2.flag = h2->flag;
          cmd.arg2.attribute = h2->attribute;
          cmd.arg2.rtyp = h2->typ;
          if (mpsr_PutLeftv(link, lv, r) != mpsr_Success) break;
#ifdef MPSR_DEBUG_DUMP
          Print("Dumped %s\n", IDID(h2));
#endif
          h2 = h2->next;
        }
#if 0
        if (IDTYP(h) == PACKAGE_CMD)
        {
          namespaceroot->pop();
        }
#endif
      }
    }

    h = h->next;
  }
  MP_EndMsg(link);
  omFreeBin(lv, sleftv_bin);
  if (rh != NULL && rh != currRingHdl) rSetHdl(rh);

  if (h == NULL && h2 == NULL)
    return mpsr_Success;
  else
    return mpsr_Failure;
}

#endif // HAVE_MPSR


