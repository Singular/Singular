/*******************************************************************
 *  File:    omOpts.h
 *  Purpose: declaration of options for omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omOpts.h,v 1.1 2000-08-14 12:18:30 obachman Exp $
 *******************************************************************/
#ifndef OM_OPTS_H
#define OM_OPTS_H

/* Default values for these options are defined in omDefaultConfig.h */
struct omOpts_s
{
  int MinTrack;
  int MinCheck;
  int Keep;
  int HowToReportErrors;
  int MarkAsStatic;
  unsigned int PagesPerRegion;
  void (*OutOfMemoryFunc)();
  void (*MemoryLowFunc)();
};
extern omOpts_t om_Opts;

#endif /* OM_OPTS_H */

