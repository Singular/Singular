/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/***************************************************************
 *  File:    p_Procs_Dynamic.h
 *  Purpose: Configuration for p_Procs as dynamic library procedures
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id: p_Procs_Dynamic.h,v 1.3 2000-12-12 08:44:49 obachman Exp $
 *******************************************************************/

#ifndef P_PROCS_DYNAMIC_H
#define P_PROCS_DYNAMIC_H

// see p_Procs_Impl.h for an explaination of these defines
#define HAVE_FAST_P_PROCS   5
#define HAVE_FAST_FIELD     2
#define HAVE_FAST_LENGTH 4
#define HAVE_FAST_ORD 4
#define HAVE_FAST_ZERO_ORD 2

// for the configuration of what goes into the kenel and what into
// shared libs, see p_Procs_Generate.cc:IsKernelProc(...)

#endif // P_PROCS_DYNAMC_H

