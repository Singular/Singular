/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    slInit_Dynamic.cc
 *  Purpose: link initialization for dynamic linking
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id$
 *******************************************************************/

/***************************************************************
 *
 * MP link Extension inits
 *
 ***************************************************************/
#include <kernel/mod2.h>
#include <Singular/silink.h>
#include <Singular/slInit.h>
#include <kernel/mod_raw.h>

#ifdef HAVE_DL
#ifdef HAVE_MPSR
#include <MP.h>
#include "mpsr_sl.h"

static void* mpsr_so_handle = NULL;
typedef void (*voidProc)();

static void* slInitMPSRHandle()
{
  if (mpsr_so_handle == NULL)
  {
    mpsr_so_handle = dynl_open_binary_warn("mpsr");
    if (mpsr_so_handle != NULL)
    {
      voidProc init_proc = (voidProc)
        dynl_sym_warn(mpsr_so_handle, "mpsr_Init");
      if (init_proc != NULL)
        (*init_proc)();
      else
        mpsr_so_handle = NULL;
    }
  }
  return mpsr_so_handle;
}

si_link_extension slInitMPFileExtension(si_link_extension s)
{
  void* handle = slInitMPSRHandle();

  if (handle == NULL) return NULL;

  s->Open = (slOpenProc) dynl_sym_warn(handle, "slOpenMPFile");
  s->Close = (slCloseProc) dynl_sym_warn(handle, "slCloseMP");
  s->Kill = (slKillProc) s->Close;
  s->Read = (slReadProc) dynl_sym_warn(handle, "slReadMP");
  s->Dump = (slDumpProc) dynl_sym_warn(handle, "slDumpMP");
  s->GetDump = (slGetDumpProc) dynl_sym_warn(handle, "slGetDumpMP");
  s->Write = (slWriteProc) dynl_sym_warn(handle, "slWriteMP");
  s->Status = (slStatusProc) dynl_sym_warn(handle, "slStatusMP");
  if (s->Open == NULL || s->Close == NULL || s->Kill == NULL ||
      s->Read == NULL || s->Dump == NULL || s->GetDump == NULL ||
      s->Write == NULL || s->Status == NULL)
    return NULL;

  s->type="MPfile";
  return s;
}

si_link_extension slInitMPTcpExtension(si_link_extension s)
{
  void* handle = slInitMPSRHandle();

  if (handle == NULL) return NULL;

  s->Open = (slOpenProc) dynl_sym_warn(handle, "slOpenMPTcp");
  s->Close = (slCloseProc) dynl_sym_warn(handle, "slCloseMP");
  s->Kill = (slKillProc) dynl_sym_warn(handle, "slKillMP");
  s->Read = (slReadProc) dynl_sym_warn(handle, "slReadMP");
  s->Dump = (slDumpProc) dynl_sym_warn(handle, "slDumpMP");
  s->GetDump = (slGetDumpProc) dynl_sym_warn(handle, "slGetDumpMP");
  s->Write = (slWriteProc) dynl_sym_warn(handle, "slWriteMP");
  s->Status = (slStatusProc) dynl_sym_warn(handle, "slStatusMP");

  if (s->Open == NULL || s->Close == NULL || s->Kill == NULL ||
      s->Read == NULL || s->Dump == NULL || s->GetDump == NULL ||
      s->Write == NULL || s->Status == NULL)
    return NULL;

  s->type="MPtcp";
  return s;
}

BatchDoProc slInitMPBatchDo()
{
  void* handle = slInitMPSRHandle();

  if (handle == NULL) return NULL;
  return (BatchDoProc) dynl_sym_warn(handle, "Batch_do");
}
#endif

#ifdef HAVE_DBM
#include <Singular/dbm_sl.h>

static void* dbm_so_handle = NULL;
static void* slInitDBMHandle()
{
  if (dbm_so_handle == NULL)
    dbm_so_handle = dynl_open_binary_warn("dbmsr");

  return dbm_so_handle;
}

si_link_extension slInitDBMExtension(si_link_extension s)
{
  void* handle = slInitDBMHandle();

  if (handle == NULL) return NULL;

  s->Open=(slOpenProc)dynl_sym_warn(handle, "dbOpen");
  s->Close=(slCloseProc)dynl_sym_warn(handle, "dbClose");
  s->Kill=(slKillProc)dynl_sym_warn(handle, "dbClose");
  s->Read=(slReadProc)dynl_sym_warn(handle, "dbRead1");
  s->Read2=(slRead2Proc)dynl_sym_warn(handle, "dbRead2");
  s->Write=(slWriteProc)dynl_sym_warn(handle, "dbWrite");

  if (s->Open == NULL || s->Close == NULL || s->Kill == NULL ||
      s->Read == NULL || s->Read2 == NULL)
    return NULL;

  s->Status=slStatusAscii;
  s->type="DBM";
  return s;
}

#endif
#endif

