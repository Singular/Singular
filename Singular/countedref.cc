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


/** @class CountedRefData
 * This class stores a reference counter as well as a Singular interpreter object.
 *
 * It also stores the Singular token number, once per type.
 **/
class CountedRefData {
 typedef CountedRefData self;

public:
  class access {
    typedef access self; 

  public:
    access(CountedRefData* data): 
      m_data(NULL), m_owns(true) { init(&data->m_data, data->m_ring);  }


    access(leftv data):
      m_data(data), m_owns(is_ref(data)) {
      if(m_owns) 
        init((CountedRefData*)(data->Data()));
    }

    access(const self& rhs):
      m_data(rhs.m_data), m_owns(rhs.m_owns) {

      if (m_owns){
        m_data = (leftv)omAlloc0(sizeof(sleftv)); 
        if(rhs.m_data != NULL) memcpy(m_data, rhs.m_data, sizeof(sleftv));
      }
    }
  
    ~access() {  if (m_owns)  omFree(m_data);  }
  
    leftv operator->() { return *this; }
    operator leftv() {  return m_data;  }

  private:

    void init(CountedRefData* all) { init(&(all->m_data), all->m_ring); }

    void init(leftv data, ring ringdata) {
      m_data = (leftv)omAlloc0(sizeof(sleftv));
      if (RingDependend(data->Typ()) && (ringdata != currRing)) {
        Werror("Can only use references from current ring.");
        return;
      }
      memcpy(m_data, data, sizeof(sleftv));
      data->next = NULL;
    }
    leftv m_data;
    bool m_owns;
  };



  /// Forbit copy construction and normal assignment
  CountedRefData(const self&);
  self& operator=(const self&);

public:
  typedef int id_type;
  typedef unsigned long count_type;

  /// Construct reference for Singular object
  CountedRefData(leftv data): m_data(*data), m_count(1), m_ring(NULL) {

    if (RingDependend(data->Typ())  && (currRing != NULL) ) {
      m_ring = currRing;
      ++m_ring->ref;
    }
    if (data->e) {
      m_data.e = (Subexpr)omAlloc0Bin(sSubexpr_bin);
      memcpy(m_data.e, data->e, sizeof(*m_data.e));
    }
  }

  /// Destructor
  ~CountedRefData()  {
    assume(m_count == 0);
    if(m_data.e) omFree(m_data.e);
    if (m_ring) --m_ring->ref;
  }


  static BOOLEAN set_to(leftv res, void* data, int op)
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
  static BOOLEAN none(leftv result) { return set_to(result, NULL, NONE); }




  /// Set Singular type identitfier 
  static void set_id(id_type new_id) {  access_id() = new_id;  }

  /// Get Singular type identitfier 
  static id_type id() { return access_id(); }

  static void* copy(self* ptr) {
    if (ptr) ptr->reclaim();
    return ptr;
  }


  static void destroy(self* ptr) {
    if(ptr && !ptr->release())
      delete ptr;
  }

  static BOOLEAN assign(leftv result, leftv arg) {
    // Case: replace assignment behind reference
    if (result->Data() != NULL)
      return iiAssign(access(result), arg);

    // Case: new reference
    if(arg->rtyp == IDHDL)
      return set_to(result, (is_ref(arg)? (self*)copy(static_cast<self*>(arg->Data())):
                             new self(arg)), id());
      
    Werror("Can only take reference from identifier");
    return none(result);
  }

  /// Check for being reference in Singular interpreter object
  static BOOLEAN is_ref(leftv arg) { return (arg->Typ() == id()); }
private:
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

  ring m_ring;
};


/// blackbox support - initialization
void* countedref_Init(blackbox*)
{
  return NULL;
}

/// blackbox support - convert to string representation
void countedref_Print(blackbox *b, void* ptr)
{
  if (ptr != NULL)
    CountedRefData::access(static_cast<CountedRefData*>(ptr))->Print();
}

/// blackbox support - convert to string representation
char* countedref_String(blackbox *b, void* ptr)
{
  if (ptr != NULL) 
    return CountedRefData::access(static_cast<CountedRefData*>(ptr))->String();
}

/// blackbox support - copy element
void* countedref_Copy(blackbox*b, void* ptr)
{ 
  return CountedRefData::copy(static_cast<CountedRefData*>(ptr));
}

/// blackbox support - assign element
BOOLEAN countedref_Assign(leftv l, leftv r)
{
  return CountedRefData::assign(l, r);
}
                                                                     

/// blackbox support - unary operations
BOOLEAN countedref_Op1(int op, leftv res, leftv head)
{
  if(op == TYPEOF_CMD)
    return CountedRefData::set_to(res, omStrDup("reference"), STRING_CMD);

  typedef CountedRefData::access access;
  return iiExprArith1(res, access(head), op);
}

/// blackbox support - binary operations
BOOLEAN countedref_Op2(int op, leftv res, leftv head, leftv arg)
{
  typedef CountedRefData::access access;
  return iiExprArith2(res, access(head), op, access(arg));
}

/// blackbox support - ternary operations
BOOLEAN countedref_Op3(int op, leftv res, leftv head, leftv arg1, leftv arg2)
{
  typedef CountedRefData::access access;
  return iiExprArith3(res, op, access(head), access(arg1), access(arg2));
}


/// blackbox support - n-ary operations
BOOLEAN countedref_OpM(int op, leftv res, leftv args)
{
  return iiExprArithM(res, CountedRefData::access(args), op);
}

/// blackbox support - destruction
void countedref_destroy(blackbox *b, void* ptr)
{
  CountedRefData::destroy(static_cast<CountedRefData*>(ptr));
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

