/*******************************************************************
 *  File:    omOpts.h
 *  Purpose: declaration of options for omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omOpts.h,v 1.5 2000-09-25 12:27:43 obachman Exp $
 *******************************************************************/
#ifndef OM_OPTS_H
#define OM_OPTS_H

/* Default values for these options are defined in omDefaultConfig.h */
struct omOpts_s
{
  int MinTrack;
  int MinCheck;
  int MaxTrack;
  int MaxCheck;
  int Keep;
  int HowToReportErrors;
  int MarkAsStatic;
  unsigned int PagesPerRegion;
  void (*OutOfMemoryFunc)();
  void (*MemoryLowFunc)();
  void (*ErrorHook)();
};
extern omOpts_t om_Opts;

#endif /* OM_OPTS_H */

