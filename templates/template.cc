// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
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
  int t = o;

  // return something
  o = a;
  return t;
};

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab
