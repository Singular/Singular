/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/***************************************************************
 *
 * File:       mpsr_Put.h
 * Purpose:    declarations for put routines for MP connection to Singular
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *  o 1/97 obachman
 *    Updated putting routines to MP and MPP v1.1
 *
 ***************************************************************/

#ifndef __MPSR_PUT__
#define __MPSR_PUT__

#include"mpsr.h"

#include <kernel/structs.h>
#include"kernel/febase.h"
#include"tok.h"
#include"ipid.h"

/***************************************************************
 * 1.) Some handy macros
 *
 ***************************************************************/
// some often used flags settings for annotations
#define MP_AnnotReqValTree                                  \
(MP_AnnotRequired | MP_AnnotValuated | MP_AnnotTreeScope)

#define MP_AnnotReqValNode                      \
(MP_AnnotRequired | MP_AnnotValuated)


/***************************************************************
 *
 * There are 4 different layers on which things are put:
 * 1.) Singular Top-level Data (chains of leftv's)
 * 2.) Leftv Data
 * 3.) Plain Singular data
 * 4.) MP primitive polynomial data
 *
 ***************************************************************/

/***************************************************************
 * 4.) Low-level polynomial data -- they should only be used internally
 *
 ***************************************************************/
extern mpsr_Status_t mpsr_PutPolyData(MP_Link_pt link, poly p, ring cring);
extern mpsr_Status_t mpsr_PutPolyVectorData(MP_Link_pt link, poly p,ring cring);
extern int          mpsr_GetNumOfRingAnnots(ring cring, BOOLEAN mv);
extern mpsr_Status_t mpsr_PutRingAnnots(MP_Link_pt link, ring cring,BOOLEAN mv);

/***************************************************************
 * 3.) Routines for Putting Plain Singular data
 *
 ***************************************************************/
// First, ring-independent data
inline mpsr_Status_t mpsr_PutInt(MP_Link_pt link, int i)
{
  mp_return(MP_PutSint32Packet(link, (MP_Sint32_t) i, 0));
}
extern mpsr_Status_t mpsr_PutIntVec(MP_Link_pt link, intvec *iv);
extern mpsr_Status_t mpsr_PutIntMat(MP_Link_pt link, intvec *iv);
inline mpsr_Status_t mpsr_PutString(MP_Link_pt link, char *str)
{
  if (strcmp(str, MPSR_QUIT_STRING) == 0)
    mp_return(MP_PutCommonOperatorPacket(link,
                                         MP_MpDict,
                                         MP_CopMpEndSession,
                                         0, 0));
  else
    mp_return(MP_PutStringPacket(link, str, 0));
}
extern mpsr_Status_t mpsr_PutRing(MP_Link_pt link, ring r);
extern mpsr_Status_t mpsr_PutProc(MP_Link_pt link, const char *pname,procinfov proc);
extern mpsr_Status_t mpsr_PutPackage(MP_Link_pt link, package pack);
inline mpsr_Status_t mpsr_PutDef(MP_Link_pt link, char *name)
{
  mp_return(MP_PutIdentifierPacket(link, MP_SingularDict, name, 0));
}
// next, ring-dependent data
extern mpsr_Status_t mpsr_PutList(MP_Link_pt link, lists l, ring cring);
extern mpsr_Status_t mpsr_PutCopCommand(MP_Link_pt link, command c, ring cring);
extern mpsr_Status_t mpsr_PutOpCommand(MP_Link_pt link, command c, ring cring);
extern mpsr_Status_t mpsr_PutNumber(MP_Link_pt link,number n,ring cring);
extern mpsr_Status_t mpsr_PutPoly(MP_Link_pt link,poly p,ring cring);
extern mpsr_Status_t mpsr_PutPolyVector(MP_Link_pt link,poly p,ring cring);
extern mpsr_Status_t mpsr_PutIdeal(MP_Link_pt link, ideal id, ring cring);
extern mpsr_Status_t mpsr_PutModule(MP_Link_pt link, ideal id, ring cring);
extern mpsr_Status_t mpsr_PutMatrix(MP_Link_pt link, ideal id, ring cring);
extern mpsr_Status_t mpsr_PutMap(MP_Link_pt link, map m, ring cring);



/***************************************************************
 * 2.) Routines for Putting Singular Leftv's
 *
 *
 ***************************************************************/
// The "Master routine, which takes any leftv, and calls the respective
// Put routine
extern mpsr_Status_t mpsr_PutLeftv(MP_Link_pt link, leftv v, ring cring);

// A handy macro which checks for the type of leftvs
#ifdef MPSR_DEBUG
#define typecheck(v,type)                           \
do if ((v)->Typ() != (type))                        \
{                                                   \
  return mpsr_SetError(mpsr_WrongLeftvType);        \
}                                                   \
while (0)
#else
#define typecheck(v,type)   ((void) 0)
#endif

// First, ring-independent data
inline mpsr_Status_t mpsr_PutIntLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, INT_CMD);
  mp_return(MP_PutSint32Packet(link, (MP_Sint32_t) v->Data(), 0));
}
inline mpsr_Status_t mpsr_PutIntVecLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, INTVEC_CMD);
  return mpsr_PutIntVec(link, (intvec *) v->Data());
}
inline mpsr_Status_t mpsr_PutIntMatLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, INTMAT_CMD);
  return mpsr_PutIntMat(link, (intvec *) v->Data());
}
inline mpsr_Status_t mpsr_PutStringLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, STRING_CMD);
  return   mpsr_PutString(link, (char *) v->Data());
}
inline mpsr_Status_t mpsr_PutRingLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, RING_CMD);
  return mpsr_PutRing(link, (ring) v->Data());
}
inline mpsr_Status_t mpsr_PutQRingLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, QRING_CMD);
  return mpsr_PutRing(link, (ring) v->Data());
}
inline mpsr_Status_t mpsr_PutProcLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, PROC_CMD);
  return mpsr_PutProc(link, v->name, (procinfov) v->Data());
}
inline mpsr_Status_t mpsr_PutDefLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, DEF_CMD);
  return mpsr_PutDef(link, (char *) v->name);
}
// now the ring-dependent leftv's
inline mpsr_Status_t mpsr_PutListLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, LIST_CMD);
  return mpsr_PutList(link, (lists) v->Data(), cring);
}
inline mpsr_Status_t mpsr_PutCommandLeftv(MP_Link_pt link, leftv v, ring cring)
{
  command cmd = (command) v->Data();

  typecheck(v, COMMAND);
  if (cmd->op == PROC_CMD)
    return mpsr_PutOpCommand(link, cmd, cring);
  else
    return mpsr_PutCopCommand(link, cmd , cring);
}
inline mpsr_Status_t mpsr_PutNumberLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, NUMBER_CMD);
  return mpsr_PutNumber(link, (number) v->Data(), cring);
}
inline mpsr_Status_t mpsr_PutPolyLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, POLY_CMD);
  return mpsr_PutPoly(link, (poly) v->Data(), cring);
}
inline mpsr_Status_t mpsr_PutIdealLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, IDEAL_CMD);
  return mpsr_PutIdeal(link, (ideal) v->Data(), cring);
}
inline mpsr_Status_t mpsr_PutVectorLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, VECTOR_CMD);
  return mpsr_PutPolyVector(link, (poly) v->Data(), cring);
}
inline mpsr_Status_t mpsr_PutMatrixLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, MATRIX_CMD);
  return mpsr_PutMatrix(link, (ideal) v->Data(), cring);
}
inline mpsr_Status_t mpsr_PutModuleLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, MODUL_CMD);
  return mpsr_PutModule(link, (ideal) v->Data(), cring);
}
inline mpsr_Status_t mpsr_PutMapLeftv(MP_Link_pt link, leftv v, ring cring)
{
  typecheck(v, MAP_CMD);
  return mpsr_PutMap(link, (map) v->Data(), cring);
}
/* inline mpsr_Status_t mpsr_PutAliasLeftv(MP_Link_pt link, leftv v) */
/* { */
/*   typecheck(v, ALIAS_CMD); */
/*   return mpsr_PutAlias(link, v->name, (idhdl) v->Data()); */
/* } */
inline mpsr_Status_t mpsr_PutPackageLeftv(MP_Link_pt link, leftv v)
{
  typecheck(v, PACKAGE_CMD);
  return mpsr_PutPackage(link, (package) v->Data());
}

/***************************************************************
 * 1.)  Singular Top-level Data (chains of leftv's)
 *      They are send as on MP mesg
 *
 ***************************************************************/
extern mpsr_Status_t mpsr_PutMsg(MP_Link_pt l, leftv v);

#endif
