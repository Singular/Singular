/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Dynamic.cc
 *  Purpose: source for dynamically loaded version of p_Procs
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id$
 *******************************************************************/
#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/p_polys.h>
#include <kernel/ring.h>
#include <kernel/p_Procs.h>
#include <kernel/p_Numbers.h>
#include <kernel/p_MemCmp.h>
#include <kernel/p_MemAdd.h>
#include <kernel/p_MemCopy.h>
#include <kernel/kbuckets.h>
#include <kernel/dError.h>

#ifdef HAVE_DL
BOOLEAN p_procs_dynamic = TRUE;

#define WARN_MSG "Singular will work properly, but much slower."

// need external linkage, so that dynl_sym works
#undef LINKAGE
#define LINKAGE extern "C"
#define p_Procs_Kernel
#include <kernel/p_Procs_Dynamic.inc>

#include <kernel/p_Procs_Dynamic.h>
// include general p_Proc stuff
#include <kernel/p_Procs_Impl.h>
#include <kernel/mod_raw.h>

// define to bound for length of p_Proc name
#define MAX_PROCNAME_LEN 200

static void* p_procs_handle_FieldIndep = NULL;
static void* p_procs_handle_FieldZp = NULL;
static void* p_procs_handle_FieldQ = NULL;
static void* p_procs_handle_FieldGeneral = NULL;

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
        return (void *)p_Copy__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Delete_Proc:
        return (void *)p_Delete__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_ShallowCopyDelete_Proc:
        return (void *)p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Mult_nn_Proc:
        return (void *)p_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_nn_Proc:
        return (void *)pp_Mult_nn__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_mm_Proc:
        return (void *)pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_mm_Noether_Proc:
        return (void *)pp_Mult_mm_Noether__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Mult_mm_Proc:
        return (void *)p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Add_q_Proc:
        return (void *)p_Add_q__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Minus_mm_Mult_qq_Proc:
        return (void *)p_Minus_mm_Mult_qq__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Neg_Proc:
        return (void *)p_Neg__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_Coeff_mm_DivSelect_Proc:
        return (void *)pp_Mult_Coeff_mm_DivSelect__FieldGeneral_LengthGeneral_OrdGeneral;
      case pp_Mult_Coeff_mm_DivSelectMult_Proc:
        return (void *)pp_Mult_Coeff_mm_DivSelectMult__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Merge_q_Proc:
        return (void *)p_Merge_q__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_kBucketSetLm_Proc:
        return (void *)p_kBucketSetLm__FieldGeneral_LengthGeneral_OrdGeneral;
      case p_Unknown_Proc:
        break;
  }
  dReportBug("p_Unknown_Proc");
  return NULL;
}

#ifdef RDEBUG
#include <omalloc.h>

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
      case pp_Mult_Coeff_mm_DivSelectMult_Proc:
        return "pp_Mult_Coeff_mm_DivSelectMult__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Merge_q_Proc:
        return "p_Merge_q__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_kBucketSetLm_Proc:
        return "p_kBucketSetLm__FieldGeneral_LengthGeneral_OrdGeneral";
      case p_Unknown_Proc:
        break;
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
    omMarkAsStaticAddr(name);
    return (void*) name;
  }
#endif
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
       GetDynamicProc(#what,  what##_Proc, field, length, ord, 1);  \
  else                                                              \
    DoReallySetProc(what, field, length, ord);                      \
}                                                                   \
while(0)
#else
#define DoSetProc DoReallySetProc
#endif


#include <kernel/p_Procs_Set.h>

#endif
