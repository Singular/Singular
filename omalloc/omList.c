/*******************************************************************
 *  File:    omList.c
 *  Purpose: implementation of routines for operations on linked lists
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omList.c,v 1.2 1999-11-22 18:12:59 obachman Exp $
 *******************************************************************/
#ifndef NULL
#define NULL ((void*) 0)
#endif

#define _VALUE(list, offset) *((void**) (list + offset))
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


