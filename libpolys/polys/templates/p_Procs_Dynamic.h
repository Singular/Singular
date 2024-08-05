/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/***************************************************************
 *  File:    p_Procs_Dynamic.h
 *  Purpose: Configuration for p_Procs as dynamic library procedures
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *******************************************************************/

#ifndef P_PROCS_DYNAMIC_H
#define P_PROCS_DYNAMIC_H

// see p_Procs_Impl.h for an explanation of these defines
#define HAVE_FAST_P_PROCS   5
#define HAVE_FAST_FIELD     2
#define HAVE_FAST_ORD 4
#define HAVE_FAST_ZERO_ORD 2

// stupid hpux -- can not handle sl which are that large
#ifdef __hpux
#define HAVE_FAST_LENGTH 3
#else
#define HAVE_FAST_LENGTH 4
#endif

// for the configuration of what goes into the kernel and what into
// shared libs, see p_Procs_Generate.cc:IsKernelProc(...)

#endif // P_PROCS_DYNAMC_H

