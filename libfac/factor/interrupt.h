///////////////////////////////////////////////////////////////////////////////
#ifndef INTERRUPT_H
#define INTERRUPT_H
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include "tmpl_inst.h"

/*BEGINPUBLIC*/
// Set this to a nonzero value to interrupt the computation
extern int libfac_interruptflag;
/*ENDPUBLIC*/

inline int
interrupt_handle( )
{
  if ( libfac_interruptflag ){ return 1; }
  return 0;
}

#endif /* INTERRUPT_H */

/*
$Log: not supported by cvs2svn $
Revision 1.3  2007/05/25 16:02:02  Singular
*hannes: removed diophant_error, format

Revision 1.2  1997/09/12 07:19:58  Singular
* hannes/michael: libfac-0.3.0


*/
