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


template <class PtrType, bool NeverNull = false, class CountType = short>
class CountedRefPtr {
  typedef CountedRefPtr self;

public:
  typedef PtrType ptr_type;
  typedef CountType count_type;

  CountedRefPtr(ptr_type ptr): m_ptr(ptr) { reclaim(); }
  CountedRefPtr(const self& rhs): m_ptr(rhs.m_ptr) { reclaim(); }

  self& operator=(ptr_type ptr) {
    kill();
    m_ptr = ptr;
    reclaim();
    return *this;
  }
  self& operator=(const self& rhs) { return operator=(rhs.m_ptr); }

  bool operator==(const self& rhs) const { return m_ptr == rhs.m_ptr; }
  bool operator==(ptr_type ptr) const { return m_ptr == ptr; }

  operator bool() const { return NeverNull || m_ptr; }
  operator ptr_type() { return m_ptr; }
  ptr_type operator->() { return *this; }

  count_type count() const { return m_ptr->ref; }

private:
  void kill() { if (release()) CountedRefPtr_kill(m_ptr); }
  
  void reclaim() { if(*this) ++m_ptr->ref; }
  count_type release() { return (*this? --m_ptr->ref: 0); }
  ptr_type m_ptr;
};


class CountedRefEnv {
  typedef CountedRefEnv self;

public:
  typedef CountedRefPtr<idhdl*> root_type;
  static idhdl* locals() {
    static idhdl myroot = NULL;
    if (myroot == NULL) {
      myroot = enterid(" _shared_data_ ", 0, PACKAGE_CMD, &IDROOT, TRUE);
    }
    return &myroot;
  }

  static idhdl newid(int typ, void* data) {
    char* name = (char*)omAlloc0(512);
    static unsigned int counter = 0;
    sprintf(name, " :%u:%p:_shared_: ", ++counter, data);

    idhdl* root = locals();
    short ref = ++(*root)->ref;

    assume((*root)->get(name, 0) == NULL); 
    (*root) = (*root)->set(name, 0, typ, FALSE);
    assume((*root) != NULL);
    IDDATA(*root) = (char*) data;
    (*root)->ref = ref;

    return *root;
  } 

  static void erase(idhdl handle) {

    idhdl* root = locals();
    short ref = --((*root)->ref);
    killhdl2(handle, root, currRing);
    (*root)->ref = ref;

    if(ref <= 0) {
      killhdl2(*root, &IDROOT, currRing);
      (*root) = NULL;
    }
 }
};


inline void CountedRefPtr_kill(ring r) { rKill(r); }


class RefCounter {
  typedef RefCounter self;

  /// Name numerical type for enumbering
  typedef unsigned long count_type;

public:
  /// Default Constructor
  RefCounter(): m_count(0) {}

  /// Copying resets the counter
  RefCounter(const self&): m_count(0) {}

  /// Destructor
  ~RefCounter() { assume(m_count == 0); }

  /// @name Reference counter management
  //@{
  count_type reclaim() { return ++m_count; }
  count_type release() { return --m_count; }
  count_type count() const { return m_count; }
  //@}

private:
  /// Number of references
  count_type m_count;
};

class LeftvShallow {
  typedef LeftvShallow self;
  
public:
  LeftvShallow(): m_data(allocate()) { }
  LeftvShallow(leftv data): 
    m_data(init(allocate(), data)) { }
  LeftvShallow(const self& rhs):
    m_data(init(allocate(), rhs.m_data)) { }

  ~LeftvShallow() {  
    kill();
    omFree(m_data);
  }
  self& operator=(leftv rhs) {
    kill();
    init(m_data, rhs);
    return *this;
  }

  self& operator=(const self& rhs) { return (*this) = rhs.m_data; }

  BOOLEAN get(leftv result) {
    leftv next = result->next;
    result->next = NULL;
    result->CleanUp();
    init(result, m_data);
    result->next = next;
    return FALSE;
  }

  /// Access to object
  leftv operator->() { return m_data;  }

protected:
  static leftv allocate() { return (leftv)omAlloc0(sizeof(sleftv)); }
  static leftv init(leftv result, leftv data) {
    memcpy(result, data, sizeof(sleftv));
    copy(result->e, data->e);
    result-> next = NULL;
    return result;
  }
  static void copy(Subexpr& current, Subexpr rhs)  {
    if (rhs == NULL) return;
    current = (Subexpr)memcpy(omAlloc0Bin(sSubexpr_bin), rhs, sizeof(*rhs));
    copy(current->next, rhs->next);
  }
  void kill() { kill(m_data->e); }
  static void kill(Subexpr current) {
    if(current == NULL) return;
    kill(current->next);
    omFree(current);
  }
protected:
  leftv m_data;
};


class LeftvDeep:
  public LeftvShallow {
  typedef LeftvDeep self;
  typedef LeftvShallow base;

public:
  LeftvDeep(): base() {}
  LeftvDeep(leftv data): base(data) { }
  LeftvDeep(const self& rhs): base(rhs) { }

  ~LeftvDeep() { m_data->CleanUp(); }

  self& operator=(const self& rhs) { return operator=(rhs.m_data); }
  self& operator=(leftv rhs) {
    assume(m_data->rtyp == IDHDL);
    kill();
    m_data->e = rhs->e;
    IDTYP((idhdl)m_data->data) =  rhs->Typ();
    IDDATA((idhdl)m_data->data) = (char*) rhs->CopyD();
    rhs->e = NULL;
    
    return *this;
  }
};


/** @class CountedRefData
 * This class stores a reference counter as well as a Singular interpreter object.
 *
 * It also stores the Singular token number, once per type.
 **/
class CountedRefData:
  public RefCounter {
  typedef CountedRefData self;
  typedef RefCounter base;

public:
  /// Construct reference for Singular object
  explicit CountedRefData(leftv data, BOOLEAN global = TRUE):
    base(), m_data(data), m_ring(parent(data)), m_global(global) { }

  /// Construct deep copy
  CountedRefData(const self& rhs):
    base(), m_data(rhs.m_data), m_ring(rhs.m_ring), m_global(rhs.m_global) { }
  
  /// Destruct
  ~CountedRefData()  { }

  /// Replace data
  self& operator=(const self& rhs) {
    m_data = rhs.m_data;
    m_global = rhs.m_global;
    m_ring = rhs.m_ring;
    return *this;
  }
 
  /// Replace with other Singular data
  void set(leftv rhs, BOOLEAN global = TRUE) {
    m_data = rhs;
    m_global = global;
    m_ring = parent(rhs);
  }

  /// Write (shallow) copy to given handle
  BOOLEAN get(leftv res) {
    reclaim();
    BOOLEAN b = broken() || m_data.get(res); 
    release();
    return b;
  }

  /// Extract (shallow) copy of stored data
  LeftvShallow operator*() { return (broken()? LeftvShallow(): m_data); }

private:
  /// Check whether identifier became invalid
  /// @note Assume that local identifiers are available
  BOOLEAN broken() {
    if (m_ring) {
      if (m_ring != currRing) 
        return complain("Referenced identifier not from current ring");    
      return m_global && brokenid(currRing->idroot) &&
        complain("Referenced identifier not available in ring anymore");  
    }
    return m_global &&
      brokenid(currPack->idroot) &&
      ((currPack == basePack) || brokenid(basePack->idroot)) &&
      complain("Referenced identifier not available in current context");
  }

  /// Raise error message and return @c TRUE
  BOOLEAN complain(const char* text) {
    Werror(text);
    return TRUE;
  }

  /// Check a given context for our identifier
  BOOLEAN brokenid(idhdl context) {
    return (context == NULL) || 
      ((context != (idhdl) m_data->data) && brokenid(IDNEXT(context)));
  }

  /// Store ring for ring-dependent objects
  static ring parent(leftv rhs) { 
    return (rhs->RingDependend()? currRing: NULL); 
  }

  /// Singular object
  LeftvDeep m_data;

  /// Store namespace for ring-dependent objects
  CountedRefPtr<ring> m_ring;

  /// Marks whether we have to check 
  bool m_global;
};

/// blackbox support - initialization
/// @note deals as marker for compatible references, too.
void* countedref_Init(blackbox*)
{
  return NULL;
}

class CountedRef {
  typedef CountedRef self;

public:
  /// name type for identifiers
  typedef int id_type;

  /// Name type for handling reference data
  typedef CountedRefData data_type;

  /// Check whether argument is already a reference type
  static BOOLEAN is_ref(leftv arg) {
    int typ = arg->Typ();
    return ((typ > MAX_TOK) &&
           (getBlackboxStuff(typ)->blackbox_Init == countedref_Init));
  }

  /// Construct new reference from Singular data  
  CountedRef(leftv arg):  m_data(new data_type(arg)) { m_data->reclaim(); }

protected:
  /// Recover previously constructed reference
  CountedRef(data_type* arg):  m_data(arg) { assume(arg); m_data->reclaim(); }

public:
  /// Construct copy
  CountedRef(const self& rhs): m_data(rhs.m_data) { m_data->reclaim(); }

  /// Replace reference
  self& operator=(const self& rhs) {
    destruct();
    m_data = rhs.m_data;
    m_data->reclaim();
    return *this;
  }

  /// Replace data that reference is pointing to
  self& operator=(leftv rhs) {
    m_data->set(rhs);
    return *this;
  }

  /// Extract (shallow) copy of stored data
  LeftvShallow operator*() { return m_data->operator*(); }

  /// Construct reference data object from
  BOOLEAN outcast(leftv result) {
    m_data->reclaim();
    if (result->rtyp == IDHDL)
      IDDATA((idhdl)result->data) = (char *)m_data;
    else
      result->data = (void *)m_data;
    return FALSE;
  }
  data_type* outcast() { 
    m_data->reclaim();
    return m_data;
  }
  /// Kills a link to the referenced object
  void destruct() { if(!m_data->release()) delete m_data; }

  /// Kills the link to the referenced object
  ~CountedRef() { destruct(); }

  /// Replaces argument by a shallow copy of the references data
  BOOLEAN dereference(leftv arg) {
    assume(is_ref(arg));
    return m_data->get(arg) || ((arg->next != NULL) && resolve(arg->next));
  }

  /// Get number of references pointing here, too
  BOOLEAN count(leftv res) { return construct(res, m_data->count() - 1); }

  /// Get internal indentifier
  BOOLEAN hash(leftv res) { return construct(res, (long)m_data); }

  /// Check for likewise identifiers
  BOOLEAN likewise(leftv res, leftv arg) {
    return resolve(arg) || construct(res, operator*()->data == arg->data); 
  }

  /// Check for identical reference objects
  BOOLEAN same(leftv res, leftv arg) { 
    return construct(res, m_data == arg->Data());
  }

  /// Get type of references data
  BOOLEAN type(leftv res) { 
    return construct(res, Tok2Cmdname(operator*()->Typ()));
  };

  /// Get (possibly) internal identifier name
  BOOLEAN name(leftv res) { return construct(res, operator*()->Name()); }

  /// Recover the actual object from Singular interpreter object
  static self cast(void* data) {
    assume(data != NULL);
    return self(static_cast<data_type*>(data));
  }

  /// Recover the actual object from raw Singular data
  static self cast(leftv arg) {
    assume((arg != NULL) && is_ref(arg));
    return self::cast(arg->Data());
  }

  /// If necessary dereference.
  static BOOLEAN resolve(leftv arg) {
    assume(arg != NULL);
    while (is_ref(arg)) { if(CountedRef::cast(arg).dereference(arg)) return TRUE; };
    return (arg->next != NULL) && resolve(arg->next);
  }

protected:

  /// Construct integer value
  static BOOLEAN construct(leftv res, long data) {
    res->data = (void*) data;
    res->rtyp = INT_CMD;
    return FALSE;
  }

  /// Construct string
  static BOOLEAN construct(leftv res, const char* data) {
    res->data = (void*)omStrDup(data);
    res->rtyp = STRING_CMD;
    return FALSE;
  }
  /// Store pointer to actual data
  data_type* m_data;
};

/// blackbox support - convert to string representation
void countedref_Print(blackbox *b, void* ptr)
{
  if (ptr) (*CountedRef::cast(ptr))->Print();
  else PrintS("<unassigned reference or shared memory>");
}

/// blackbox support - convert to string representation
char* countedref_String(blackbox *b, void* ptr)
{
  if (ptr == NULL) return omStrDup(sNoName);
  return (*CountedRef::cast(ptr))->String();
}

/// blackbox support - copy element
void* countedref_Copy(blackbox*b, void* ptr)
{ 
  if (ptr) return CountedRef::cast(ptr).outcast();
  return NULL;
}

/// blackbox support - assign element
BOOLEAN countedref_Assign(leftv result, leftv arg)
{
  // Case: replace assignment behind reference
  if (result->Data() != NULL) {
    return CountedRef::cast(result).dereference(result) ||
      CountedRef::resolve(arg) ||
      iiAssign(result, arg);
  }
  
  // Case: new reference
  if(arg->rtyp == IDHDL) 
    return (result->Typ() == arg->Typ()?
            CountedRef::cast(arg):
            CountedRef(arg)).outcast(result);

  Werror("Can only take reference from identifier");
  return FALSE;
}

BOOLEAN countedref_CheckInit(leftv res, leftv arg)
{
  if (arg->Data() != NULL) return FALSE;
  res->rtyp = NONE;
  Werror("Noninitialized access");
  return TRUE;
}
                                                                 
/// blackbox support - unary operations
BOOLEAN countedref_Op1(int op, leftv res, leftv head)
{
  if(op == TYPEOF_CMD)
    return blackboxDefaultOp1(op, res, head);

  if (countedref_CheckInit(res, head)) return TRUE;

  if(op == head->Typ()) {
    res->rtyp = op;
    return iiAssign(res, head);
  }

  return CountedRef::cast(head).dereference(head) || 
    iiExprArith1(res, head, op == DEF_CMD? head->Typ(): op);
}

/// blackbox support - binary operations
BOOLEAN countedref_Op2(int op, leftv res, leftv head, leftv arg)
{
  return countedref_CheckInit(res, head) ||
    CountedRef::cast(head).dereference(head) || CountedRef::resolve(arg) ||
    iiExprArith2(res, head, op, arg);
}

/// blackbox support - ternary operations
BOOLEAN countedref_Op3(int op, leftv res, leftv head, leftv arg1, leftv arg2)
{
  return countedref_CheckInit(res, head) ||
    CountedRef::cast(head).dereference(head) || 
    CountedRef::resolve(arg1) || CountedRef::resolve(arg2) ||
    iiExprArith3(res, op, head, arg1, arg2);
}



/// blackbox support - destruction
void countedref_destroy(blackbox *b, void* ptr)
{
  if (ptr) CountedRef::cast(ptr).destruct();
}


class CountedRefShared:
  public CountedRef {
  typedef CountedRefShared self;
  typedef CountedRef base;
public:
  /// Construct new reference from Singular data  
  CountedRefShared(leftv arg):  base(new data_type(wrap(arg), FALSE)) { }

private:
  /// Recover previously constructed shared data
  CountedRefShared(data_type* arg):  base(arg) { }
  CountedRefShared(const base& rhs):  base(rhs) { }
public:
  /// Construct copy
  CountedRefShared(const self& rhs): base(rhs) { }

  ~CountedRefShared() {  kill(); }

  self& operator=(const self& rhs) {
    kill();
    base::operator=(rhs);
    return *this;
  }

  /// Replace data that reference is pointing to
  self& operator=(leftv rhs) {
    m_data->set(rhs, FALSE);
    return *this;
  }
  void destruct() {
    kill();
    base::destruct();
  }

  static self cast(leftv arg) { return base::cast(arg); }
  static self cast(void* arg) { return base::cast(arg); }
private:

  static leftv wrap(leftv arg) {
    idhdl handle = CountedRefEnv::newid(arg->Typ(), arg->CopyD()); 
    arg->CleanUp();
    arg->data = handle;
    arg->rtyp = IDHDL;
    arg->name = omStrDup(IDID(handle));
    return arg;
  }

 void kill() {
   if (m_data->count() > 1) return;
   
   LeftvShallow data = base::operator*();
   CountedRefEnv::erase((idhdl)data->data);
   data->data = NULL;
   data->rtyp = NONE;
 }
};


/// blackbox support - n-ary operations
BOOLEAN countedref_OpM(int op, leftv res, leftv args)
{
  if (args->Data() == NULL) return FALSE;

  if(op == SYSTEM_CMD) {
    if (args->next) {
      leftv next = args->next;
      args->next = NULL;
      CountedRef obj = CountedRef::cast(args);
      char* name = (next->Typ() == STRING_CMD? 
                    (char*) next->Data(): (char*)next->Name());
      next = next->next;
      if (next) {
        if (strcmp(name, "same") == 0) return obj.same(res, next);
        if (strncmp(name, "like", 4) == 0) return obj.likewise(res, next);
      }
      if (strncmp(name, "count", 5) == 0) return obj.count(res);
      if (strcmp(name, "hash") == 0) return obj.hash(res);
      if (strcmp(name, "name") == 0) return obj.name(res);
      if (strncmp(name, "type", 4) == 0) return obj.type(res);
    }
    return TRUE;
  }
  return CountedRef::cast(args).dereference(args) || iiExprArithM(res, args, op);
}
/// blackbox support - assign element
BOOLEAN countedref_AssignShared(leftv result, leftv arg)
{
  /// Case: replace assignment behind reference
  if ((result->Data()) != NULL) {
    if (CountedRefShared::resolve(arg)) return TRUE;
    CountedRefShared::cast(result) = arg;
    return FALSE;
  }
  
  /// Case: new reference to already shared data
  if (result->Typ() == arg->Typ()) 
    return CountedRefShared::cast(arg).outcast(result);

  /// Case: new shared data
  return CountedRefShared(arg).outcast(result);
}

/// blackbox support - destruction
void countedref_destroyShared(blackbox *b, void* ptr)
{
  if (ptr) CountedRefShared::cast(ptr).destruct();
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
  setBlackboxStuff(bbx, "reference");

  /// The @c shared type is "inherited" from @c reference.
  /// It just uses another constructor (to make its own copy of the).
  blackbox *bbxshared = 
    (blackbox*)memcpy(omAlloc(sizeof(blackbox)), bbx, sizeof(blackbox));
  bbxshared->blackbox_Assign  = countedref_AssignShared;
  bbxshared->blackbox_destroy  = countedref_destroyShared;

  setBlackboxStuff(bbxshared, "shared");
}

extern "C" { void mod_init() { countedref_init(); } }

