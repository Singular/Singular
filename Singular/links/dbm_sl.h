/*!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
**************************************************************
 *  File:    dbm_sl.h
 *  Purpose: declaration of sl_link routines for dbm
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 ******************************************************************
*/
LINKAGE BOOLEAN dbOpen(si_link l, short flag, leftv u);
LINKAGE BOOLEAN dbWrite(si_link l, leftv v);
LINKAGE leftv dbRead1(si_link l);
LINKAGE leftv dbRead2(si_link l, leftv key);
LINKAGE BOOLEAN dbClose(si_link l);

