/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Dynamic.cc
 *  Purpose: source for dynamically loaded version of p_Procs
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Procs_Dynamic.cc,v 1.1 2000-12-07 15:03:59 obachman Exp $
 *******************************************************************/
#include "mod2.h"
#include "structs.h"
#include "p_polys.h"
#include "ring.h"
#include "p_Procs.h"
#include "p_Numbers.h"
#include "p_MemCmp.h"
#include "p_MemAdd.h"
#include "p_MemCopy.h"
#include "kbuckets.h"
#include "dError.h"

BOOLEAN p_procs_dynamic = TRUE;

// no external linkage
#define LINKAGE 
#include "p_Procs_Dynamic.inc"

#include "p_Procs_Dynamic.h"
// include general p_Proc stuff
#include "p_Procs_Impl.h"
#include "mod_raw.h"

// define to bound for length of p_Proc name
#define MAX_PROCNAME_LEN 200

static void* p_procs_handle = NULL;
void InitSetDynamicProcs()
{
  if (p_procs_handle == NULL)
    p_procs_handle =  dynl_open_binary_warn("p_Procs");
}
#define InitSetProcs(f, l, o) InitSetDynamicProcs()


void* GetGeneralProc(p_Proc proc)
{
  switch(proc)
  {
      case p_Copy_Proc: 
        return p_Copy__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Delete_Proc: 
        return p_Delete__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_ShallowCopyDelete_Proc: 
        return p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Mult_nn_Proc: 
        return p_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_nn_Proc: 
        return pp_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_mm_Proc: 
        return pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_mm_Noether_Proc: 
        return pp_Mult_mm_Noether__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Mult_mm_Proc: 
        return p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Add_q_Proc: 
        return p_Add_q__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Minus_mm_Mult_qq_Proc: 
        return p_Minus_mm_Mult_qq__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Neg_Proc: 
        return p_Neg__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_Coeff_mm_DivSelect_Proc: 
        return pp_Mult_Coeff_mm_DivSelect__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Merge_q_Proc: 
        return p_Merge_q__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_kBucketSetLm_Proc: 
        return p_kBucketSetLm__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Unknown_Proc:
        break;
  }
  dReportBug("p_Unknown_Proc");
  return NULL;
}

#ifdef RDEBUG
#include "omalloc.h"

char* GetGeneralProcName(p_Proc proc)
{
  switch(proc)
  {
      case p_Copy_Proc: 
        return "p_Copy__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Delete_Proc: 
        return "p_Delete__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_ShallowCopyDelete_Proc: 
        return "p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Mult_nn_Proc: 
        return "p_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_nn_Proc: 
        return "pp_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_mm_Proc: 
        return "pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_mm_Noether_Proc: 
        return "pp_Mult_mm_Noether__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Mult_mm_Proc: 
        return "p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Add_q_Proc: 
        return "p_Add_q__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Minus_mm_Mult_qq_Proc: 
        return "p_Minus_mm_Mult_qq__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Neg_Proc: 
        return "p_Neg__FieldGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_Coeff_mm_DivSelect_Proc: 
        return "pp_Mult_Coeff_mm_DivSelect__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Merge_q_Proc: 
        return "p_Merge_q__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_kBucketSetLm_Proc: 
        return "p_kBucketSetLm__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Unknown_Proc:
        break;
  }
  return "p_Unknown_Proc";
}
char* GetDynamicProcName(const char* proc_s, p_Proc proc, 
                         p_Field field, p_Length length, p_Ord ord)
{
  void* proc_ptr = NULL;
  char proc_name[MAX_PROCNAME_LEN];
  
  if (p_procs_handle != NULL && 
      (field != FieldGeneral || length != LengthGeneral || ord != OrdGeneral))
  {
    sprintf(proc_name, "%s__%s_%s_%s", proc_s, 
            p_FieldEnum_2_String(field), 
            p_LengthEnum_2_String(length), 
            p_OrdEnum_2_String(ord));
    proc_ptr = dynl_sym_warn(p_procs_handle, proc_name);
    if (proc_ptr != NULL) 
    {
      char* name = omStrDup(proc_name);
      omMarkAsStaticAddr(name);
      return name;
    }
  }
  return GetGeneralProcName(proc);
}
  
#endif


void* GetDynamicProc(const char* proc_s, p_Proc proc, 
                     p_Field field, p_Length length, p_Ord ord)
{
  void* proc_ptr = NULL;
  
  if (p_procs_handle != NULL && 
      (field != FieldGeneral || length != LengthGeneral || ord != OrdGeneral))
  {
    char proc_name[MAX_PROCNAME_LEN];
    sprintf(proc_name, "%s__%s_%s_%s", proc_s, 
            p_FieldEnum_2_String(field), 
            p_LengthEnum_2_String(length), 
            p_OrdEnum_2_String(ord));
    proc_ptr = dynl_sym(p_procs_handle, proc_name);
  }
  if (proc_ptr == NULL)
    proc_ptr = GetGeneralProc(proc);

  return  proc_ptr;
}
  

#define DoReallySetProc(what, field, length, ord)           \
  _p_procs->what = (what##_Proc_Ptr)                        \
     GetDynamicProc(#what, what##_Proc, field, length, ord)

#ifdef RDEBUG
#define DoSetProc(what, field, length, ord)                         \
do                                                                  \
{                                                                   \
  if (set_names)                                                    \
    _p_procs->what = (what##_Proc_Ptr)                              \
       GetDynamicProcName(#what,  what##_Proc, field, length, ord); \
  else                                                              \
    DoReallySetProc(what, field, length, ord);                      \
}                                                                   \
while(0)
#else
#define DoSetProc DoReallySetProc
#endif

      
#include "p_Procs_Set.h"




