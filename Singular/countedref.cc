// -*- c++ -*-
//*****************************************************************************
/** @file countedref.cc
 *
 * @author Alexander Dreyer
 * @date 2010-12-15
 *
 * This file defines the @c blackbox operations for the countedref type.
 *
 * @par Copyright:
 *   (c) 2010 by The Singular Team, see LICENSE file
**/
//*****************************************************************************





#include <Singular/mod2.h>

#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <Singular/newstruct.h>

#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>

#include "lists.h"
#include "attrib.h"


class sleftv_shallow {
  typedef sleftv_shallow self;
public:
  sleftv_shallow(sleftv& data): m_data(data) { }
  sleftv_shallow(const self& rhs): m_data(rhs.m_data) {}

  ~sleftv_shallow() { }// do not need the clean up, do not own
  
  leftv operator->() { return *this; }
  operator leftv() { return &m_data; }

private:
  sleftv m_data; 
};
class ListElementRefData;
/** @class CountedRefData
 * This class stores a reference counter as well as a Singular interpreter object.
 *
 * It also stores the Singular token number, once per type.
 **/
class CountedRefData {
 typedef CountedRefData self;

  /// Forbit copy construction
  CountedRefData(const self&);
  self& operator=(const self&);

public:
  typedef int id_type;
  typedef unsigned long count_type;

  /// Construct reference for Singular object
  CountedRefData(leftv data): m_data(*data), m_count(1) {
    if (data->e) {
      m_data.e = (Subexpr)omAlloc0Bin(sSubexpr_bin);
      memcpy(m_data.e, data->e, sizeof(*m_data.e));
    }
  }

  /// Destructor
  ~CountedRefData()  { if(m_data.e) omFree(m_data.e); };

  sleftv_shallow get() {
    return sleftv_shallow(m_data);
  }

  static BOOLEAN construct(leftv res, leftv arg) {
    return extract(res, (is_ref(arg)? (self*)arg->Data(): new self(arg)), id());
  }

  static BOOLEAN extract(leftv res, void* data, int op)
  {
    if (res->rtyp == IDHDL) {
      IDDATA((idhdl)res->data) = (char *)data;
      IDTYP((idhdl)res->data)  = op;
    }
    else {
      res->data = (void *)data;
      res->rtyp = op;
    }
    return (op == NONE? TRUE: FALSE);
  }
  /// @name Forward operations
  //@{
  BOOLEAN operator()(int op, leftv result, leftv arg){
      return iiExprArith2(result, get(), op,
                        (is_ref(arg)? cast(arg).get(): arg));
    
  }

  BOOLEAN operator()(int op, leftv result, leftv arg1, leftv arg2) {
    return iiExprArith3(result, op, get(), 
                        (is_ref(arg1)? cast(arg1).get(): arg1),
                        (is_ref(arg2)? cast(arg2).get(): arg2));
  }
  //@}

  /// Set Singular type identitfier 
  static void set_id(id_type new_id) {  access_id() = new_id;  }

  /// Get Singular type identitfier 
  static id_type id() { return access_id(); }

  /// Extract Singular interpreter object
  static self& cast(leftv value, leftv next=NULL) {
    assume((value != NULL) && is_ref(value));
    return cast((void*)value->Data(), next);
  }

  /// Extract reference from Singular interpreter object data
  static self& cast(void* data, leftv next=NULL) {
    assume(data != NULL);
    self* result = static_cast<CountedRefData*>(data);
    result->m_data.next=next;   /// @todo resolve refs in tail
    return *result;
  }

  /// Check for being reference in Singular interpreter object
  static BOOLEAN is_ref(leftv arg) { return (arg->Typ() == id()); }

  /// @name Reference counter management
  //@{
  count_type reclaim() { return ++m_count; }
  count_type release() { return --m_count; }
  count_type count() const { return m_count; }
  //@}

private:

  /// Access identifier (one per class)
  static id_type& access_id() {
    static id_type g_id = 0;
    return g_id;
  }

  /// Reference counter
  count_type m_count;

  /// Singular object
  sleftv m_data;
};


/// blackbox support - initialization
void* countedref_Init(blackbox*)
{
  return NULL;
}

/// blackbox support - convert to string representation
void countedref_Print(blackbox *b, void* ptr)
{
  CountedRefData::cast(ptr).get()->Print();
}

/// blackbox support - convert to string representation
char* countedref_String(blackbox *b, void* ptr)
{
  CountedRefData::cast(ptr).get()->String();
}

/// blackbox support - copy element
void* countedref_Copy(blackbox*b, void* ptr)
{ 
  CountedRefData::cast(ptr).reclaim();
  return ptr;
}

/// blackbox support - assign element
BOOLEAN countedref_Assign(leftv l, leftv r)
{
  if (l->Data() == NULL)        // Case: new reference
  {
    if(r->rtyp != IDHDL) {
      Werror("Can only take reference from identifier");
      return CountedRefData::extract(l, NULL, NONE);
    }
    return CountedRefData::construct(l, r);
  }

  // Case: replace assignment behind reference
  return iiAssign(CountedRefData::cast(l).get(), r);
}
                                                                     

/// blackbox support - unary operations
BOOLEAN countedref_Op1(int op, leftv res, leftv head)
{
  if(op == TYPEOF_CMD)
    return CountedRefData::extract(res, omStrDup("reference"), STRING_CMD);

  return iiExprArith1(res, CountedRefData::cast(head).get(), op);
}

/// blackbox support - binary operations
BOOLEAN countedref_Op2(int op, leftv res, leftv head, leftv arg)
{
  return CountedRefData::cast(head)(op, res, arg);
}

/// blackbox support - ternary operations
BOOLEAN countedref_Op3(int op, leftv res, leftv head, leftv arg1, leftv arg2)
{
   return CountedRefData::cast(head)(op, res, arg1, arg2);
}


/// blackbox support - n-ary operations
BOOLEAN countedref_OpM(int op, leftv res, leftv args)
{
  return iiExprArithM(res, CountedRefData::cast(args, args->next).get(), op);
}

/// blackbox support - destruction
void countedref_destroy(blackbox *b, void* ptr)
{
  if (ptr) {
    CountedRefData* pRef = static_cast<CountedRefData*>(ptr);
    if(!pRef->release())
      delete pRef;
  }
}

void countedref_init() 
{
  blackbox *bbx = (blackbox*)omAlloc0(sizeof(blackbox));
  bbx->blackbox_destroy = countedref_destroy;
  bbx->blackbox_String  = countedref_String;
  bbx->blackbox_Print  = countedref_Print;
  bbx->blackbox_Init    = countedref_Init;
  bbx->blackbox_Copy    = countedref_Copy;
  bbx->blackbox_Assign  = countedref_Assign;
  bbx->blackbox_Op1     = countedref_Op1;
  bbx->blackbox_Op2     = countedref_Op2;
  bbx->blackbox_Op3     = countedref_Op3;
  bbx->blackbox_OpM     = countedref_OpM;
  bbx->data             = omAlloc0(newstruct_desc_size());
  CountedRefData::set_id(setBlackboxStuff(bbx,"reference"));
}

extern "C" { void mod_init() { countedref_init(); } }

