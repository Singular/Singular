///////////////////////////////////////////////////////////////////////////////
// $Id$

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
