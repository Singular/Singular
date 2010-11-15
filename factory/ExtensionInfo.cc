/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file ExtensionInfo.cc
 *
 * This file provides member functions for ExtensionInfo
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "ExtensionInfo.h"

ExtensionInfo::ExtensionInfo (const bool extension)
{
  m_alpha= Variable (1);
  m_beta= Variable (1);
  m_gamma= CanonicalForm ();
  m_delta= CanonicalForm ();
  m_GFDegree= 1;
  m_GFName= 'Z';
  m_extension= extension;
}

ExtensionInfo::ExtensionInfo (const Variable& alpha, const Variable& beta,
                              const CanonicalForm& gamma, const CanonicalForm&
                              delta, const int nGFDegree, const char cGFName,
                              const bool extension)
{
  m_alpha= alpha;
  m_beta= beta;
  m_gamma= gamma;
  m_delta= delta;
  m_GFDegree= nGFDegree;
  m_GFName= cGFName;
  m_extension= extension;
}

ExtensionInfo::ExtensionInfo (const Variable& alpha, const Variable& beta,
                              const CanonicalForm& gamma, const CanonicalForm&
                              delta)
{
  m_alpha= alpha;
  m_beta= beta;
  m_gamma= gamma;
  m_delta= delta;
  m_GFDegree= 0;
  m_GFName= 'Z';
  m_extension= true;
}

ExtensionInfo::ExtensionInfo (const Variable& alpha, const bool extension)
{
  m_alpha= alpha;
  m_beta= Variable (1);
  m_gamma= CanonicalForm ();
  m_delta= CanonicalForm ();
  m_GFDegree= 0;
  m_GFName= 'Z';
  m_extension= extension;
}

ExtensionInfo::ExtensionInfo (const Variable& alpha)
{
  m_alpha= alpha;
  m_beta=  Variable (1);
  m_gamma= CanonicalForm ();
  m_delta= CanonicalForm ();
  m_GFDegree= 1;
  m_GFName= 'Z';
  m_extension= true;
}

ExtensionInfo::ExtensionInfo (const int nGFDegree, const char cGFName, const
                              bool extension)
{
  m_alpha= Variable (1);
  m_beta= Variable (1);
  m_gamma= CanonicalForm ();
  m_delta= CanonicalForm ();
  m_GFDegree= nGFDegree;
  m_GFName= cGFName;
  m_extension= extension;
}

