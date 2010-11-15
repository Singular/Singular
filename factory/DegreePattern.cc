/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file DegreePattern.cc
 *
 * This file provides functions for manipulating DegreePatterns
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "DegreePattern.h"
#include "cf_iter.h"
#include "ftmpl_functions.h"
#include "gfops.h"


DegreePattern::DegreePattern (const CFList& l)
{
  m_data = NULL;

  if (l.length() == 0)
    m_data = new Pattern();
  else
  {

  Variable x= Variable (1);
  int p= getCharacteristic();
  int d= getGFDegree();
  char cGFName= gf_name;
  setCharacteristic(0);
  CanonicalForm buf= 1;
  CFListIterator k= l;
  for (int i= 0; i < l.length(); i++, k++)
    buf *= (power (x, degree (k.getItem(), x)) + 1);

  int j= 0;
  for (CFIterator i= buf; i.hasTerms(); i++, j++)
    ;

  ASSERT ( j > 1, "j > 1 expected" );

  m_data = new Pattern( j - 1 );

  int i= 0;
  for (CFIterator m = buf; i < getLength(); i++, m++)
    (*this) [i]= m.exp();

  if (d > 1)
    setCharacteristic (p, d, cGFName);
  else
    setCharacteristic (p);
  }
}


void DegreePattern::intersect (const DegreePattern& degPat)
{
  if (degPat.getLength() < getLength())
  {
    DegreePattern bufDeg= *this;
    *this= degPat;
    return (*this).intersect (bufDeg);
  }

  int count= 0;
  int length= tmin (getLength(), degPat.getLength());
  int* buf= new int [length];
  for (int i= 0; i < length; i++)
  {
    if (degPat.find ((*this)[i]))
    {
      buf[i]= (*this)[i];
      count++;
    }
    else
      buf[i]= -1;
  }
  ASSERT ( count > 0, "count > 0 expected" );

  init (count);
  count= 0;
  for (int i= 0; i < length; i++)
  {
    if (buf[i] != -1)
    {
      (*this) [count]= buf[i];
      count++;
    }
  }
  delete[] buf;
}

void DegreePattern::refine ()
{
  if (getLength() <= 1)
    return;
  int count= 0;
  int* buf= new int [getLength()];
  int d= (*this) [0];
  int pos;
  for (int i= 0; i < getLength(); i++)
    buf[i]= -1;
  for (int i= 1; i < getLength(); i++)
  {
    pos= (*this).find (d - (*this)[i]);
    if (pos)
    {
      buf[i]= (*this)[i];
      count++;
    }
  }
  buf[0]= d;
  count++;
  if (count == getLength())
  {
    return;
  }
  int length= getLength();

  ASSERT ( count > 0, "count > 0 expected" );
  init (count);
  count= 0;
  for (int i= 0; i < length; i++)
  {
    if (buf[i] != -1)
    {
      (*this)[count]= buf[i];
      count++;
    }
  }

  delete[] buf;
  return;
}

