/*******************************************************************
 *  File:    omList.h
 *  Purpose: declaration of routines for operations on linked lists
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omList.h,v 1.1.1.1 1999-11-18 17:45:53 obachman Exp $
 *******************************************************************/

#ifndef OM_LIST_H
#define OM_LIST_H


#define OM_LIST_OFFSET(ptr, name_of_offset_field) \
  (ptr != NULL ? ((void*) &(ptr->name_of_offset_field)) - ((void*) ptr) : 0)

/********************************************************************
 *
 * Primitve routines -- don't use directly, use om macros, instead
 *
 ********************************************************************/
/* Returns the length of a memory list; assumes list has no cycles */
int _omListLength(void* list, int next);
/* Returns last non-NULL element of list; assumes list has no cycles */
void* _omListLast(void* list, int next);
/* Checks whether memory list has cycles: If yes, returns address of
 * first element of list which is contained at least twice in memory
 * list. If no, NULL is returned */
void* _omListHasCycle(void* list, int next);
/* returns addr, if addr is contained in memory list
 * 0, otherwise */
void* _omIsOnList(void* list, int next, void* addr);
/* Removes addr from list, if contained in it and returns new list */
void* _omRemoveFromList(void* list, int next, void* addr);
/* 
 * The following cast list->long_field to a pointer to  unsigned long 
 */
/* Find element with that value in list and return it, if found (else NULL) */
void* _omFindInList(void* list, int next, int long_field, 
                    unsigned long what);
/* 
 * The following assume that list is ordered increasingly w.r.t. long_field
 */
/* Finds element in sorted list */
void* _omFindInSortedList(void* list, int next, int long_field, 
                          unsigned long what);
/* Remove element with that value from list and return new list */
void* _omRemoveFromSortedList(void* list,int next,int long_field,
                                  void* addr);
/* Inserts addr at the right place, returns list assumes addr != NULL */
void* _omInsertInSortedList(void* list, int next, int long_field, 
                            void* addr);


/********************************************************************
 *
 * The following routines assume that Next(list) == *((void**) list)
 *
 ********************************************************************/
#define omListLength(ptr) \
  _omListLength(ptr, 0)
#define omListLast(ptr) \
  _omListLast(ptr, 0)
#define omListHasCycle(ptr) \
  _omListHasCycle(ptr, 0)
#define omIsOnList(ptr, addr) \
  _omIsOnList(ptr, 0, addr)
#define omRemoveFromList(ptr, addr) \
  _omRemoveFromList(ptr, 0, addr)
#define omFindInList(ptr, what, value) \
  _omFindInList(ptr, 0, OM_LIST_OFFSET(ptr, what), value)
#define omInsertInSortedList(ptr, what, addr) \
  _omInsertInSortedList(ptr, 0, OM_LIST_OFFSET(ptr, what), addr)
#define omFindInSortedList(ptr, what, value) \
  _omFindInSortedList(ptr, 0, OM_LIST_OFFSET(ptr, what), value)
#define omRemoveFromSortedList(ptr, what, addr) \
  _omRemoveFromSortedList(ptr, 0, OM_LIST_OFFSET(ptr, what), addr)


/********************************************************************
 *
 * The following routines have name of next field as argument
 *
 ********************************************************************/
#define omGListLength(ptr, next) \
  _omListLength(ptr, OM_LIST_OFFSET(ptr, next))
#define omGListLast(ptr, next) \
  _omListLast(ptr, OM_LIST_OFFSET(ptr, next))
#define omGListHasCycle(ptr, next) \
  _omListHasCycle(ptr, OM_LIST_OFFSET(ptr, next))
#define omIsOnGList(ptr, next, addr) \
  _omIsOnList(ptr, OM_LIST_OFFSET(ptr, next), addr)
#define omRemoveFromGList(ptr, next, addr) \
  _omRemoveFromList(ptr, OM_LIST_OFFSET(ptr, next), addr)
#define omFindInGList(ptr, next, what, value) \
  _omFindInList(ptr, OM_LIST_OFFSET(ptr, next), OM_LIST_OFFSET(ptr, what), value)
#define omInsertInSortedGList(ptr, next, what, addr) \
  _omInsertInSortedList(ptr, OM_LIST_OFFSET(ptr, next), OM_LIST_OFFSET(ptr, what), addr)
#define omFindInSortedGList(ptr, next, what, value) \
  _omFindInSortedList(ptr, OM_LIST_OFFSET(ptr, next),OM_LIST_OFFSET(ptr,what),value)
#define omRemoveFromSortedGList(ptr, next, what, addr) \
  _omRemoveFromSortedList(ptr, OM_LIST_OFFSET(addr,next),OM_LIST_OFFSET(addr,what),addr)

#endif /* OM_LIST_H */
