/*******************************************************************
 *  File:    omTrack.h
 *  Purpose: declaration routines for getting Backtraces of stack
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omTrack.h,v 1.1.1.1 1999-11-18 17:45:54 obachman Exp $
 *******************************************************************/
#ifndef OM_TRACK_H
#define OM_TRACK_H

#include <stdio.h>

/* writes into void** up to max_frames return addresses of current stack,
   excludes addr of this function, returns count of addr actually
   determined */
int omGetCurrentBackTrace(void** addr, int max_frames);
/* prints max_frames addreses, translated by addr2line to fd */
int omPrintBackTrace(void** addr, int max_frames, FILE* fd);
/* needs to be called from main with argv[0] as argument */
int omPrintCurrentBackTrace(int from_frame, int max_frames, FILE *fd);
void omInitTrack(const char* argv0);

#endif /* OM_TRACK_H */
