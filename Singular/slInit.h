/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    slInit.h
 *  Purpose: declarations of link initialization functions
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *******************************************************************/
#ifndef SL_INIT_H
#define SL_INIT_H

#include <Singular/silink.h>

#ifdef HAVE_DBM
si_link_extension slInitDBMExtension(si_link_extension s);
#endif

#endif // SL_INIT_H
