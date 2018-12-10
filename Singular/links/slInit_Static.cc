/*!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
**************************************************************
 *  File:    slInit_Static.cc
 *  Purpose: link initialization for static linking
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 ******************************************************************
*/




#include "kernel/mod2.h"

#include "Singular/links/silink.h"
#include "Singular/links/slInit.h"

#ifdef HAVE_DBM

# ifndef USE_GDBM
#   include "Singular/links/dbm_sl.h"
# else
#   include "Singular/links/sing_dbm.h"
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
#endif /*!
 #ifdef HAVE_DBM 
*/
