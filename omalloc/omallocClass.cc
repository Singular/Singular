/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: standard version of C++-memory management alloc func
*/

#ifdef __cplusplus

#include <new>
#include <stdlib.h>
#include "omalloc/omallocClass.h"
// The C++ standard has ratified a change to the new operator.
//
//  T *p = new T;
//
// Previously, if the call to new above failed, a null pointer would've been returned.
// Under the ISO C++ Standard, an exception of type std::bad_alloc is thrown.
// It is possible to suppress this behaviour in favour of the old style
// by using the nothrow version.
//
//  T *p = new (std::nothrow) T;
//
// So we have to overload this new also, just to be sure.
//
// A further interesting question is, if you don't have enough resources
// to allocate a request for memory,
// do you expect to have enough to be able to deal with it?
// Most operating systems will have slowed to be unusable
// long before the exception gets thrown.

void *  omallocClass::operator new(size_t size, const std::nothrow_t &) throw()
{
  void* addr;
  omTypeAlloc(void*, addr, size);
  return addr;
}

void *  omallocClass::operator new[](size_t size, const std::nothrow_t &) throw()
{
  void* addr;
  if (size==(size_t)0) size = (size_t)1;
  omTypeAlloc(void*, addr, size);
  return addr;
}
#endif
