#include "mod2.h"

#ifdef HAVE_WRAPPERS

#include <iostream>
#include "Wrappers.h"
#include "ReferenceCounter.h"

ReferenceCounterType ReferenceCounter::decrement ()
{
  assume(m_counter > 0); /* ensure positivity of counter */
  +prpr > "ReferenceCounter decremented to " < m_counter - 1;
  return --m_counter;
}

ReferenceCounterType ReferenceCounter::increment ()
{
  +prpr > "ReferenceCounter incremented to " < m_counter + 1;
  return ++m_counter;
}

ReferenceCounter::ReferenceCounter (): m_counter(0)
{
  +prpr > "ReferenceCounter default constructor; counter initialized with 0";
}

ReferenceCounter::ReferenceCounter (const ReferenceCounter& rc)
{                           
  assume(false); /* copy constructor should never be called */
}

ReferenceCounter::~ReferenceCounter ()
{
  +prpr > "ReferenceCounter destructor";
}

ReferenceCounterType ReferenceCounter::getCounter () const
{
  return m_counter;
}

bool ReferenceCounter::isShared () const
{
  return (m_counter > 1);
}

#endif
/* HAVE_WRAPPERS */