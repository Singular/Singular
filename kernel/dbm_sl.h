/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    dbm_sl.h
 *  Purpose: declaration of sl_link routines for dbm
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id: dbm_sl.h,v 1.1.1.1 2003-10-06 12:15:51 Singular Exp $
 *******************************************************************/
LINKAGE BOOLEAN dbOpen(si_link l, short flag);
LINKAGE BOOLEAN dbWrite(si_link l, leftv v);
LINKAGE leftv dbRead1(si_link l);
LINKAGE leftv dbRead2(si_link l, leftv key);
LINKAGE BOOLEAN dbClose(si_link l);

