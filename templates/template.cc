/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file template.cc
 * 
 * This file implements the class Template. 
 *
 * ABSTRACT: We need this for something...
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

// include header file
#include "template.h"

USING_NAMESPACE_SINGULARXX

// compiled stuff of the class Template

/// We use Algorithm ABC from Book...
int Template::someMethod( int a, int &o )
{
  int b;

  // return something
  o = 2;
  return -1;
};
