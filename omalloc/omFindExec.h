/*******************************************************************
 *  File:    omFindExec.h
 *  Purpose: declaration routines for getting Backtraces of stack
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_FIND_EXEC_H
#define OM_FIND_EXEC_H

/* determines absolute pathname of program prog, writes it into exec */
/* returns exec on succes, NULL on failure */
char* omFindExec(const char* prog, char* exec);

#endif /* OM_FINDEXEC_H */
