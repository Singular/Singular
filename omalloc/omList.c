/*******************************************************************
 *  File:    omList.c
 *  Purpose: implementation of routines for operations on linked lists
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include "omConfig.h"

#ifdef HAVE_OMALLOC

#ifndef NULL
#define NULL ((void*) 0)
#endif

#define _VALUE(list, offset) *((void**) ((char *)list + offset))
#define VALUE(list, offset) (unsigned long) _VALUE(list, offset)
#define NEXT(list) _VALUE(list, next)
#define ITER(list) list = NEXT(list)

int _omListLength(void* list, int next)
{
  int l = 0;
  while (list != NULL)
  {
    l++;
    ITER(list);
  }
  return l;
}


void* _omListLast(void* list, int next)
{
  if (list == NULL) return NULL;

  while (NEXT(list) != NULL) ITER(list);

  return list;
}


void* _omListHasCycle(void* list, int next)
{
  void* l1 = list;
  void* l2;

  int l = 0, i;

  while (l1 != NULL)
  {
    i = 0;
    l2 = list;
    while (l1 != l2)
    {
      i++;
      ITER(l2);
    }
    if (i != l) return l1;
    ITER(l1);
    l++;
  }
  return NULL;
}


void* _omIsOnList(void* list, int next, void* addr)
{
  if (addr == NULL)
    return (NULL);

  while (list != NULL)
  {
    if (addr == list) return addr;
    ITER(list);
  }
  return 0;
}

void* _omRemoveFromList(void* list, int next, void* addr)
{
  void* nlist;
  void* olist;

  if (list == NULL) return NULL;

  nlist = NEXT(list);
  if (list == addr) return nlist;

  olist = list;
  while (nlist != NULL && nlist != addr)
  {
    list = nlist;
    ITER(nlist);
  }

  if (nlist != NULL) NEXT(list) = NEXT(nlist);
  return olist;
}

void* _omFindInList(void* list, int next, int long_field, unsigned long what)
{
  while (list != NULL)
  {
    if (VALUE(list, long_field) == what) return list;
    ITER(list);
  }
  return NULL;
}

void* _omFindInSortedList(void* list, int next, int long_field,
                          unsigned long what)
{
  while (list != NULL)
  {
    if (VALUE(list, long_field) >= what)
    {
      if (VALUE(list, long_field) == what) return list;
      return NULL;
    }
    ITER(list);
  }
  return NULL;
}

void* _omRemoveFromSortedList(void* list, int next, int long_field, void* addr)
{
  void* nlist;
  void* olist;
  unsigned long what = VALUE(addr, long_field);

  if (list == NULL) return NULL;
  nlist = NEXT(list);
  if (list == addr) return nlist;
  if (VALUE(list, long_field) > what) return list;

  olist = list;
  while (nlist != NULL && nlist != addr)
  {
    if (VALUE(list, long_field) > what) return olist;
    list = nlist;
    ITER(nlist);
  }

  if (nlist != NULL) NEXT(list) = NEXT(nlist);
  return olist;
}

void* _omInsertInSortedList(void* list, int next, int long_field, void* addr)
{
  unsigned long what = VALUE(addr, long_field);

  if (list == NULL || what <= VALUE(list, long_field))
  {
    NEXT(addr) = list;
    return addr;
  }
  else
  {
    void* prev = list;
    void* curr = NEXT(list);

    while (curr != NULL && VALUE(curr, long_field) < what)
    {
      prev = curr;
      ITER(curr);
    }
    NEXT(prev) = addr;
    NEXT(addr) = curr;
    return list;
  }
}


#ifndef OM_NDEBUG
#include "omalloc.h"
#include "omDebug.h"

omError_t _omCheckList(void* list, int next, int level, omError_t report, OM_FLR_DECL)
{
  if (level < 1) return omError_NoError;

  if (level == 1)
  {
    while (list != NULL)
    {
      omCheckReturn(omCheckPtr(list, report, OM_FLR_VAL));
      ITER(list);
    }
  }
  else
  {
    void* l1 = list;
    void* l2;
    int l = 0, i;

    l1 = list;
    while (l1 != NULL)
    {
      omCheckReturn(omCheckPtr(l1, report, OM_FLR_VAL));
      i = 0;
      l2 = list;
      while (l1 != l2)
      {
        i++;
        ITER(l2);
      }
      if (i != l)
        return omReportError(omError_ListCycleError, report, OM_FLR_VAL, "");
      ITER(l1);
      l++;
    }
  }
  return omError_NoError;
}

omError_t _omCheckSortedList(void* list, int next, int long_field, int level, omError_t report, OM_FLR_DECL)
{
  void* prev = NULL;

  if (level <= 1) return omError_NoError;

  if (level == 1)
  {
    while (list != NULL)
    {
      omCheckReturn(omCheckPtr(list, report, OM_FLR_VAL));
      if (prev != NULL && VALUE(prev, long_field) > VALUE(list, long_field))
        return omReportError(omError_SortedListError, report, OM_FLR_VAL,
                             "%d > %d", VALUE(prev, long_field), VALUE(list, long_field));
      prev = list;
      ITER(list);
    }
  }
  else
  {
    void* l1 = list;
    void* l2;
    int l = 0, i;

    while (l1 != NULL)
    {
      omCheckReturn(omCheckPtr(l1, report, OM_FLR_VAL));
      if (prev != NULL && VALUE(prev, long_field) > VALUE(l1, long_field))
        return omReportError(omError_SortedListError, report, OM_FLR_VAL,
                             "%d > %d", VALUE(prev, long_field), VALUE(l1, long_field));
      i = 0;
      l2 = list;
      while (l1 != l2)
      {
        i++;
        ITER(l2);
      }
      omCheckReturnError(i != l, omError_ListCycleError);
      prev = l1;
      ITER(l1);
      l++;
    }
  }
  return omError_NoError;
}
#endif
#endif
