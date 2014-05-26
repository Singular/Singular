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


#ifdef HAVE_NAMESPACES

#define BEGIN_NAMESPACE(a) namespace a {
#define END_NAMESPACE }

#define USING_NAMESPACE(a) using namespace a;
#define FROM_NAMESPACE(a, s) a :: s

#else

#define BEGIN_NAMESPACE(a) 
#define END_NAMESPACE 

#define USING_NAMESPACE(a)
#define FROM_NAMESPACE(a, s) s

#endif


#define SINGULARXXNAME Singular

#define BEGIN_NAMESPACE_SINGULARXX BEGIN_NAMESPACE(SINGULARXXNAME)
#define END_NAMESPACE_SINGULARXX END_NAMESPACE

#define USING_NAMESPACE_SINGULARXX USING_NAMESPACE(SINGULARXXNAME)
#define FROM_NAMESPACE_SINGULARXXNAME(s) FROM_NAMESPACE(SINGULARXXNAME, s)

#define BEGIN_NAMESPACE_NONAME BEGIN_NAMESPACE()



#ifndef HAVE_THROW
#  define SINGULARXX_THROW(type) throw type();
#else
#  define SINGULARXX_THROW(type) Werror(type().what());
#endif

/// For optimizing if-branches
#ifdef __GNUC__
#define LIKELY(expression) (__builtin_expect(!!(expression), 1))
#define UNLIKELY(expression) (__builtin_expect(!!(expression), 0))
#else
#define LIKELY(expression) (expression)
#define UNLIKELY(expression) (expression)
#endif 

// #include "CSingularTypes.h"

#endif /* SINGULAR_singular_defs_h_ */
