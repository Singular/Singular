/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Dynamic.cc
 *  Purpose: source for dynamically loaded version of p_Procs
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *******************************************************************/
#include "misc/auxiliary.h"

#include "factory/factory.h"

#include "reporter/reporter.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

#include "polys/kbuckets.h"

#include "polys/templates/p_Numbers.h"

#include "polys/templates/p_Procs.h"
#include "polys/templates/p_MemCmp.h"
#include "polys/templates/p_MemAdd.h"
#include "polys/templates/p_MemCopy.h"


#ifdef HAVE_DL
const BOOLEAN p_procs_dynamic = TRUE;

#define WARN_MSG "Singular will work properly, but much slower."

// need external linkage, so that dynl_sym works
#undef LINKAGE
#define LINKAGE extern "C"
#define p_Procs_Kernel

#include "templates/p_Procs.inc"

#include "templates/p_Procs_Dynamic.h"
// include general p_Proc stuff
#include "templates/p_Procs_Impl.h"

#include "mod_raw.h"

// define to bound for length of p_Proc name
#define MAX_PROCNAME_LEN 200

STATIC_VAR void* p_procs_handle_FieldIndep = NULL;
STATIC_VAR void* p_procs_handle_FieldZp = NULL;
STATIC_VAR void* p_procs_handle_FieldQ = NULL;
STATIC_VAR void* p_procs_handle_FieldGeneral = NULL;

static void* p_ProcInitHandle(void** handle, const char* module)
{
  if (*handle == NULL)
  {
    char name[25];
    sprintf(name, "p_Procs_%s", module);
    *handle = dynl_open_binary_warn(name, WARN_MSG);
  }
  return *handle;
}

static inline void* p_ProcGetHandle(p_Proc proc, p_Field field)
{
  const char* module =  p_ProcField_2_Module(proc, field);

  if (strcmp(module, "FieldIndep") == 0)
    return p_ProcInitHandle(&p_procs_handle_FieldIndep, module);
  else if (strcmp(module, "FieldZp") == 0)
    return p_ProcInitHandle(&p_procs_handle_FieldZp, module);
  else if (strcmp(module, "FieldQ") == 0)
    return p_ProcInitHandle(&p_procs_handle_FieldQ, module);
  else if (strcmp(module, "FieldGeneral") == 0)
    return p_ProcInitHandle(&p_procs_handle_FieldGeneral, module);
  else
  {
    assume(0);
    return NULL;
  }
}


static void* GetGeneralProc(p_Proc proc)
{
  switch(proc)
  {
      case p_Copy_Proc:
        return cast_A_to_vptr(p_Copy__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_Delete_Proc:
        return cast_A_to_vptr(p_Delete__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_ShallowCopyDelete_Proc:
        return cast_A_to_vptr(p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_Add_q_Proc:
        return cast_A_to_vptr(p_Add_q__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_Neg_Proc:
        return cast_A_to_vptr(p_Neg__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_Merge_q_Proc:
        return cast_A_to_vptr(p_Merge_q__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_kBucketSetLm_Proc:
        return cast_A_to_vptr(p_kBucketSetLm__FieldGeneral_LengthGeneral_OrdGeneral);
#ifdef HAVE_RINGS
      case p_Mult_nn_Proc:
        return cast_A_to_vptr(p_Mult_nn__RingGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_nn_Proc:
        return cast_A_to_vptr(pp_Mult_nn__RingGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_mm_Proc:
        return cast_A_to_vptr(pp_Mult_mm__RingGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_mm_Noether_Proc:
        return cast_A_to_vptr(pp_Mult_mm_Noether__RingGeneral_LengthGeneral_OrdGeneral);
      case p_Mult_mm_Proc:
        return cast_A_to_vptr(p_Mult_mm__RingGeneral_LengthGeneral_OrdGeneral);
      case p_Minus_mm_Mult_qq_Proc:
        return cast_A_to_vptr(p_Minus_mm_Mult_qq__RingGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_Coeff_mm_DivSelect_Proc:
        return cast_A_to_vptr(pp_Mult_Coeff_mm_DivSelect__RingGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_Coeff_mm_DivSelectMult_Proc:
        return cast_A_to_vptr(pp_Mult_Coeff_mm_DivSelectMult__RingGeneral_LengthGeneral_OrdGeneral);
#else
      case p_Mult_nn_Proc:
        return cast_A_to_vptr(p_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_nn_Proc:
        return cast_A_to_vptr(pp_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_mm_Proc:
        return cast_A_to_vptr(pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_mm_Noether_Proc:
        return cast_A_to_vptr(pp_Mult_mm_Noether__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_Mult_mm_Proc:
        return cast_A_to_vptr(p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral);
      case p_Minus_mm_Mult_qq_Proc:
        return cast_A_to_vptr(p_Minus_mm_Mult_qq__FieldGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_Coeff_mm_DivSelect_Proc:
        return cast_A_to_vptr(pp_Mult_Coeff_mm_DivSelect__FieldGeneral_LengthGeneral_OrdGeneral);
      case pp_Mult_Coeff_mm_DivSelectMult_Proc:
        return cast_A_to_vptr(pp_Mult_Coeff_mm_DivSelectMult__FieldGeneral_LengthGeneral_OrdGeneral);
#endif
      case p_Unknown_Proc:
        break;
  }
  dReportBug("p_Unknown_Proc");
  return NULL;
}

#ifdef RDEBUG
static const char* GetGeneralProcName(p_Proc proc)
{
  switch(proc)
  {
      case p_Copy_Proc:
        return "p_Copy__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Delete_Proc:
        return "p_Delete__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_ShallowCopyDelete_Proc:
        return "p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Add_q_Proc:
        return "p_Add_q__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Neg_Proc:
        return "p_Neg__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Merge_q_Proc:
        return "p_Merge_q__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_kBucketSetLm_Proc:
        return "p_kBucketSetLm__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Unknown_Proc:
        break;
#ifdef HAVE_RINGS
      case p_Mult_nn_Proc:
        return "p_Mult_nn__RingGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_nn_Proc:
        return "pp_Mult_nn__RingGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_mm_Proc:
        return "pp_Mult_mm__RingGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_mm_Noether_Proc:
        return "pp_Mult_mm_Noether__RingGeneral_LengthGeneral_OrdGeneral";
      case p_Mult_mm_Proc:
        return "p_Mult_mm__RingGeneral_LengthGeneral_OrdGeneral";
      case p_Minus_mm_Mult_qq_Proc:
        return "p_Minus_mm_Mult_qq__RingGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_Coeff_mm_DivSelect_Proc:
        return "pp_Mult_Coeff_mm_DivSelect__RingGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_Coeff_mm_DivSelectMult_Proc:
        return "pp_Mult_Coeff_mm_DivSelectMult__RingGeneral_LengthGeneral_OrdGeneral";
#else
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
      case p_Minus_mm_Mult_qq_Proc:
        return "p_Minus_mm_Mult_qq__FieldGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_Coeff_mm_DivSelect_Proc:
        return "pp_Mult_Coeff_mm_DivSelect__FieldGeneral_LengthGeneral_OrdGeneral";
      case pp_Mult_Coeff_mm_DivSelectMult_Proc:
        return "pp_Mult_Coeff_mm_DivSelectMult__FieldGeneral_LengthGeneral_OrdGeneral";
#endif
  }
  return "p_Unknown_Proc";
}
#endif


static void* GetDynamicProc(const char* proc_s, p_Proc proc,
                            p_Field field, p_Length length, p_Ord ord
#ifdef RDEBUG
                     , int get_name = 0
#endif
                     )
{
  void* proc_ptr = NULL;
  char proc_name[MAX_PROCNAME_LEN];
  sprintf(proc_name, "%s__%s_%s_%s", proc_s,
          p_FieldEnum_2_String(field),
          p_LengthEnum_2_String(length),
          p_OrdEnum_2_String(ord));
  // printf("set %s\n",proc_name);
  // first, try to get the proc from the kernel
  proc_ptr = dynl_sym(DYNL_KERNEL_HANDLE, proc_name);
  if (proc_ptr == NULL)
  {
    proc_ptr = dynl_sym_warn(p_ProcGetHandle(proc, field), proc_name, WARN_MSG);
    // last but not least use general proc
    if (proc_ptr == NULL)
    {
      proc_ptr = GetGeneralProc(proc);
#ifdef RDEBUG
      sprintf(proc_name,"%s", GetGeneralProcName(proc));
#endif
    }
  }
#ifdef RDEBUG
  if (get_name)
  {
    char* name = omStrDup(proc_name);
#if (!defined(SING_NDEBUG)) && (!defined(OM_NDEBUG))
    omMarkAsStaticAddr(name);
#endif
    return (void*) name;
  }
#endif
  return  proc_ptr;
}


#define DoReallySetProc(what, field, length, ord)           \
  _p_procs->what =    cast_vptr_to_A<what##_Proc_Ptr>(      \
     GetDynamicProc(#what, what##_Proc, field, length, ord))

#ifdef RDEBUG
#define DoSetProc(what, field, length, ord)                         \
do                                                                  \
{                                                                   \
  if (set_names)                                                    \
    _p_procs->what =    cast_vptr_to_A<what##_Proc_Ptr>(            \
       GetDynamicProc(#what,  what##_Proc, field, length, ord, 1));  \
  else                                                              \
    DoReallySetProc(what, field, length, ord);                      \
}                                                                   \
while(0)
#else
#define DoSetProc DoReallySetProc
#endif

#include "templates/p_Procs_Set.h"

#endif

