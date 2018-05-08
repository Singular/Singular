// -*- c++ -*-
//*****************************************************************************
/** @file singularxx_defs.h
 *
 * @author Alexander Dreyer
 * @date 2009-06-15
 *
 * This file includes some basic definitions for the SINGULAR++ interface.
 *
 * @par Copyright:
 *   (c) 2009 by The SINGULAR Team, see LICENSE file
 *
**/
//*****************************************************************************

// Note: inclusion of CSINGULARTypes.h at the end


// Get configuration
// #include "singular_config.h"

#ifndef SINGULAR_singular_defs_h_
#define SINGULAR_singular_defs_h_


#define FROM_NAMESPACE(a, s) s

#ifndef HAVE_THROW
#  define SINGULARXX_THROW(type) throw type();
#else
#  define SINGULARXX_THROW(type) WerrorS(type().what());
#endif

#endif /* SINGULAR_singular_defs_h_ */
