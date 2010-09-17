/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    slInit_Static.cc
 *  Purpose: link initialization for static linking
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

#ifdef HAVE_MPSR

#include <MP.h>
#include <Singular/mpsr_sl.h>

extern void mpsr_Init();

si_link_extension slInitMPFileExtension(si_link_extension s)
{
  mpsr_Init();
  s->Open=slOpenMPFile;
  s->Close=slCloseMP;
  s->Kill=slCloseMP;
  s->Read=slReadMP;
  s->Dump=slDumpMP;
  s->GetDump=slGetDumpMP;
  s->Write=slWriteMP;
  s->Status=slStatusMP;
  s->type="MPfile";
  return s;
}

si_link_extension slInitMPTcpExtension(si_link_extension s)
{
  mpsr_Init();
  s->Open=slOpenMPTcp;
  s->Close=slCloseMP;
  s->Kill=slKillMP;
  s->Read=slReadMP;
  s->Dump=slDumpMP;
  s->GetDump=slGetDumpMP;
  s->Write=slWriteMP;
  s->Status=slStatusMP;
  s->type="MPtcp";
  return s;
}

BatchDoProc slInitMPBatchDo()
{
  mpsr_Init();
  return Batch_do;
}
#endif

#ifdef HAVE_DBM
#ifndef USE_GDBM
#include <Singular/dbm_sl.h>
#else
#include <Singular/sing_dbm.h>
#endif
si_link_extension slInitDBMExtension(si_link_extension s)
{
  s->Open=dbOpen;
  s->Close=dbClose;
  s->Kill=dbClose;
  s->Read=dbRead1;
  s->Read2=dbRead2;
  s->Write=dbWrite;
  s->Status=slStatusAscii;
  s->type="DBM";
  return s;
}
#endif
