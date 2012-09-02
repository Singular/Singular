/// -*- c++ -*-
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

/** @class CountedRefPtr
 * This class implements a smart pointer which handles pointer-style access
 * to a reference-counted structure and destructing the latter after use.
 *
 * The template arguments, include the pointer type @c PtrType, and two
 * integral (bool) properties: use @c isWeak to disallow destruction
 * and @c NeverNull to assume, that @c PtrType cannot be @c NULL.
 * Finally, @c CountType allows you to select a typ to represent the internal reference count.
 *
 * @note The class of @c PtrType must have an accessible integral attribute @c ref.
 * For convenience use @c RefCounter as public base.
 * In addition you must overload @c void CountedRefPtr_kill(PtrType) accordingly.
 **/
template <class PtrType, bool isWeak = false, bool NeverNull = false, class CountType = short>
class CountedRefPtr {
  typedef CountedRefPtr self;

public:
  //{ @name Name template arguments
  typedef PtrType ptr_type;
  typedef CountType count_type;
  enum { is_weak = isWeak, never_null = NeverNull };
  //}

  /// Default constructor @note: exisis only if @c NeverNull is false
  CountedRefPtr(): m_ptr(NULL) {}

  /// Convert from pointer
  CountedRefPtr(ptr_type ptr): m_ptr(ptr) { reclaim(); }

  /// Convert from compatible smart pointer
  template <bool Never>
  CountedRefPtr(const CountedRefPtr<ptr_type, !is_weak, Never, count_type>& rhs):
    m_ptr(rhs.m_ptr) { reclaim(); }

  /// Construct refernce copy
  CountedRefPtr(const self& rhs):
    m_ptr(rhs.m_ptr) { reclaim(); }

  /// Unlink one reference
  ~CountedRefPtr() { release(); }

  //{ @name Replace data behind reference
  self& operator=(const self& rhs) { return operator=(rhs.m_ptr); }
  self& operator=(ptr_type ptr) {
    release();
    m_ptr = ptr;
    reclaim();
    return *this;
  }
  //}

  /// Checking equality 
  bool operator==(const self& rhs) const { return m_ptr == rhs.m_ptr; }

  //{ @name Pointer-style interface
  bool operator==(ptr_type ptr) const { return m_ptr == ptr; }
  operator bool() const { return NeverNull || m_ptr; }
  operator const ptr_type() const { return m_ptr; }
  operator ptr_type() { return m_ptr; }
  const ptr_type operator->() const { return *this; }
  ptr_type operator->() { return *this; }
  //}

  //{ @name Reference count interface
  count_type count() const { return (*this? m_ptr->ref: 0); }
  void reclaim() { if (*this) ++m_ptr->ref; }
  void release() { 
    if (*this && (--m_ptr->ref <= 0) && !is_weak)
      CountedRefPtr_kill(m_ptr); 
  }
  //}

private:
  /// Store actual pointer
  ptr_type m_ptr;
};

/// Default constructor only implemented if @c NeverNull is false
//template <class PtrType, bool isWeak,bool val,class CountType>
//inline CountedRefPtr<PtrType, isWeak, val, CountType>::CountedRefPtr():
//  m_ptr(NULL) { }

/** @class RefCounter
 * This class implements implements a refernce counter which we can use
 * as a public base of objects managed by @CountedRefPtr.
 **/
class RefCounter {

public:
  /// Name numerical type for enumbering
  typedef short count_type;

  /// Allow our smart pointer to access internals
  template <class, bool, bool, class> friend class CountedRefPtr;

  /// Any Constructor resets the counter
  RefCounter(...): ref(0) {}

  /// Destructor
  ~RefCounter() { assume(ref == 0); }

private:
  /// Number of references
  count_type ref;  // naming consistent with other classes
};

class CountedRefEnv {
  typedef CountedRefEnv self;

public:
  static leftv idify(leftv head, idhdl* root) {
    idhdl handle = newid(head, root);
    leftv res = (leftv)omAlloc0(sizeof(*res));
    res->data =(void*) handle;
    res->rtyp = IDHDL;
    return res;
  }

  static idhdl newid(leftv head, idhdl* root) {

    static unsigned int counter = 0;
    char* name = (char*) omAlloc0(512);
    sprintf(name, " :%u:%p:_shared_: ", ++counter, head->data);
    if ((*root) == NULL )
      enterid(name, 0, head->rtyp, root, TRUE, FALSE);
    else
      *root = (*root)->set(name, 0, head->rtyp, TRUE);

    IDDATA(*root) = (char*) head->data;
    return *root;
  }

  static void clearid(idhdl handle, idhdl* root) {
    IDDATA(handle)=NULL;
    IDTYP(handle)=NONE;
    killhdl2(handle, root, NULL);
  }
  static int& ref_id() {
    static int g_ref_id = 0;
    return g_ref_id;
  }

  static int& sh_id() {
    static int g_sh_id = 0;
    return g_sh_id;
  }
};

/// Overloading ring destruction
inline void CountedRefPtr_kill(ring r) { rKill(r); }

class LeftvHelper {
public:
  template <class Type>
  static Type* cpy(Type* result, Type* data)  {
    return (Type*)memcpy(result, data, sizeof(Type));
  }
  template <class Type>
  static Type* cpy(Type* data)  {
    return cpy((Type*)omAlloc0(sizeof(Type)), data);
  }
  template <class Type>
  static Type* recursivecpy(Type* data)  {
    if (data == NULL) return data;
    Type* result = cpy(data);
    result->next = recursivecpy(data->next);
    return result;
  }
  template <class Type>
  static Type* shallowcpy(Type* result, Type* data)  {
    cpy(result, data)->e = recursivecpy(data->e);
    return result;
  }
  template <class Type>
  static Type* shallowcpy(Type* data)  {
    return shallowcpy((Type*) omAlloc0(sizeof(Type)), data);
  }
  template <class Type>
  static void recursivekill(Type* current) {
    if(current == NULL) return;
    recursivekill(current->next);
    omFree(current);
  }
  static leftv allocate() { return (leftv)omAlloc0(sizeof(sleftv)); }

};

class LeftvShallow:
  public LeftvHelper {
  typedef LeftvShallow self;
  
public:
  LeftvShallow(): m_data(allocate()) { }
  LeftvShallow(leftv data): m_data(shallowcpy(data)) { }
  LeftvShallow(const self& rhs):  m_data(shallowcpy(rhs.m_data)) { }

  ~LeftvShallow() {  
    recursivekill(m_data->e);
    omFree(m_data);
  }
  self& operator=(leftv rhs) {
    recursivekill(m_data->e);
    shallowcpy(m_data, rhs);
    return *this;
  }

  self& operator=(const self& rhs) { return (*this) = rhs.m_data; }

  /// Access to object
  const leftv operator->() const { return m_data;  }
  leftv operator->() { return m_data;  }

protected:
  leftv m_data;
};


class LeftvDeep: public LeftvHelper {
  typedef LeftvDeep self;

  /// @name Do not permit copying
  //@{
  self& operator=(const self&);
  LeftvDeep(const self&);
  //@}

public:
  struct copy_tag {};
  LeftvDeep(): m_data(allocate()) {}
  LeftvDeep(leftv data): m_data(cpy(data)) { 
    data->e = NULL;   // occupy subexpression 
    if(!isid()) m_data->data=data->CopyD(); 
  }

  LeftvDeep(leftv data, copy_tag): m_data(allocate()) {  m_data->Copy(data);  }

  ~LeftvDeep() { m_data->CleanUp(); }
  operator LeftvShallow() { return m_data;}

  bool like(const self& rhs) const { return m_data->data == rhs.m_data->data; }

  self& operator=(leftv rhs) {
    if(isid()) {
      m_data->e = rhs->e;
      rhs->e = NULL;
      IDTYP((idhdl)m_data->data) =  rhs->Typ();
      IDDATA((idhdl)m_data->data) = (char*) rhs->CopyD();
    }
    else {
      m_data->CleanUp();
      m_data->Copy(rhs);
    }
    return *this;
  }

  /// Check a given context for our identifier
  BOOLEAN brokenid(idhdl context) const {
    assume(isid());
    return (context == NULL) || 
      ((context != (idhdl) m_data->data) && brokenid(IDNEXT(context)));
  }

  /// Put a shallow copy to given @c leftv
  BOOLEAN put(leftv result) {
    leftv next = result->next;
    result->next = NULL;
    result->CleanUp();

    shallowcpy(result, m_data);
    result->next = next;
    return FALSE;
  }

  /// Get additional data (e.g. subexpression data) from likewise instances
  BOOLEAN retrieve(leftv res) {
    if (res->data == m_data->data)  {
      if(m_data->e != res->e) recursivekill(m_data->e);
      cpy(m_data, res);
      res->Init();
      return TRUE;
    }
    return FALSE;
  }



  BOOLEAN isid() const { return m_data->rtyp==IDHDL;}
  BOOLEAN ringed() { return m_data->RingDependend(); }
  BOOLEAN unassigned() const { return m_data->Typ()==0; }

  leftv idify(idhdl* root) {
    leftv res = (isid()? m_data: CountedRefEnv::idify(m_data, root));
    ++(((idhdl)res->data)->ref);
    return res;
  }


  void clearid(idhdl* root) {
    assume(isid());
    if (--((idhdl)m_data->data)->ref <= 0)  // clear only if we own
      CountedRefEnv::clearid((idhdl)m_data->data, root);
  }

private:
  leftv m_data;
};

/** @class CountedRefData
 * This class stores a reference counter as well as a Singular interpreter object.
 *
 * It also stores the Singular token number, once per type.
 **/
class CountedRefData:
  public RefCounter {
  typedef CountedRefData self;
  typedef CountedRefPtr<self*, false, true> self_ptr;
  typedef RefCounter base;

  /// Generate object linked to other reference (e.g. for subscripts)
  CountedRefData(leftv wrapped, self_ptr back):
    base(), m_data(wrapped), m_ring(back->m_ring), m_back(back) {  }

  /// @name  isallow copying to avoid inconsistence
  //@{
  self& operator=(const self&);
  CountedRefData(const self&);
  //@}
public:
  typedef LeftvDeep::copy_tag copy_tag;

  /// Fix smart pointer type to referenced data
  typedef self_ptr ptr_type;

  /// Fix smart pointer type to ring
  typedef CountedRefPtr<ring, true> ring_ptr;

  /// Construct shared memory empty Singular object
  explicit CountedRefData():
    base(), m_data(), m_ring(), m_back() { }

  /// Reference Singular object
  explicit CountedRefData(leftv data):
    base(), m_data(data), m_ring(parent(data)), m_back() { }

  /// Construct reference for Singular object
  CountedRefData(leftv data, copy_tag do_copy):
    base(), m_data(data, do_copy), m_ring(parent(data)), m_back() { }

  /// Destruct
  ~CountedRefData() {  if (m_back)  m_data.clearid(root()); }

  /// Generate 
  ptr_type subscripted() {
    return new self(m_data.idify(root()), 
                    m_back? m_back: ptr_type(this));
  }

  /// Replace with other Singular data
  self& operator=(leftv rhs) {
    m_data = rhs;
    m_ring = parent(rhs);
    return *this;
  }

  /// Write (shallow) copy to given handle
  BOOLEAN put(leftv res) { return broken() || m_data.put(res);  }

  /// Extract (shallow) copy of stored data
  LeftvShallow operator*() const { return (broken()? LeftvShallow(): (const LeftvShallow&)m_data); }

  /// Determine active ring when ring dependency changes
  BOOLEAN rering() {
    if (m_ring ^ m_data.ringed()) m_ring = (m_ring? NULL: currRing);
    return (m_back && m_back->rering());
  }

  /// Get the current context
  idhdl* root() { return  (m_ring? &m_ring->idroot: &IDROOT); }

  /// Check whether identifier became invalid
  BOOLEAN broken() const {
    if (m_ring) {
      if (m_ring != currRing) 
        return complain("Referenced identifier not from current ring");    

      return m_data.isid()  && m_data.brokenid(currRing->idroot) &&
        complain("Referenced identifier not available in ring anymore");  
    }
    
    if (!m_data.isid()) return FALSE;
    return  m_data.brokenid(IDROOT) &&
     ((currPack == basePack) ||  m_data.brokenid(basePack->idroot)) &&
     complain("Referenced identifier not available in current context");
  }

  /// Reassign actual object
  BOOLEAN assign(leftv result, leftv arg) {
    if (!m_data.isid()) {
      (*this) = arg;
      return FALSE;
    }
    return put(result) || iiAssign(result, arg) || rering();
  }
  /// Recover additional information (e.g. subexpression) from likewise object
  BOOLEAN retrieve(leftv res) { return m_data.retrieve(res); }

  /// Check whether data is all-zero 
  BOOLEAN unassigned() const { return m_data.unassigned(); }

private:
  /// Raise error message and return @c TRUE
  BOOLEAN complain(const char* text) const  {
    Werror(text);
    return TRUE;
  }

  /// Store ring for ring-dependent objects
  static ring parent(leftv rhs) { 
    return (rhs->RingDependend()? currRing: NULL); 
  }

protected:
  /// Singular object
  LeftvDeep m_data;

  /// Store namespace for ring-dependent objects
  ring_ptr m_ring;

  /// Reference to actual object for indexed structures  
  ptr_type m_back;
};

/// Supporting smart pointer @c CountedRefPtr
inline void CountedRefPtr_kill(CountedRefData* data) { delete data; }


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

  /// Name type for handling referenced data
  typedef CountedRefData data_type;

  /// Fix smart pointer type to referenced data
  typedef data_type::ptr_type data_ptr;

  /// Check whether argument is already a reference type
  static BOOLEAN is_ref(leftv arg) {
    int typ = arg->Typ();
    return ((typ==CountedRefEnv::ref_id())  ||(typ==CountedRefEnv::sh_id()) );
    //    return ((typ > MAX_TOK) &&
    //            (getBlackboxStuff(typ)->blackbox_Init == countedref_Init));
  }

  /// Reference given Singular data  
  explicit CountedRef(leftv arg):  m_data(new data_type(arg)) { }

protected:
  /// Recover previously constructed reference
  CountedRef(data_ptr arg):  m_data(arg) { assume(arg); }

public:
  /// Construct copy
  CountedRef(const self& rhs): m_data(rhs.m_data) { }

  /// Replace reference
  self& operator=(const self& rhs) {
    m_data = rhs.m_data;
    return *this;
  }

  BOOLEAN assign(leftv result, leftv arg) { 
    return m_data->assign(result,arg);
  }

  /// Extract (shallow) copy of stored data
  LeftvShallow operator*() { return m_data->operator*(); }

  /// Construct reference data object marked by given identifier number
  BOOLEAN outcast(leftv res, int typ) {
    res->rtyp = typ;
    return outcast(res);
  }

  /// Construct reference data object from *this
  BOOLEAN outcast(leftv res) {
    if (res->rtyp == IDHDL)
      IDDATA((idhdl)res->data) = (char *)outcast();
    else
      res->data = (void *)outcast();
    return FALSE;
  }

  /// Construct raw reference data
  data_type* outcast() { 
    m_data.reclaim();
    return m_data;
  }

  /// Kills a link to the referenced object
  void destruct() { m_data.release(); }

  /// Kills the link to the referenced object
  ~CountedRef() { }

  /// Replaces argument by a shallow copy of the references data
  BOOLEAN dereference(leftv arg) {
    m_data.reclaim();
    BOOLEAN b= m_data->put(arg) || ((arg->next != NULL) && resolve(arg->next));
    m_data.release();
    return b;
  }

  /// Check whether object in valid in current context
  BOOLEAN broken() {return m_data->broken(); }

  /// Check whether (shared) data was initialized but not assigned yet.
  BOOLEAN unassigned() const { return m_data->unassigned(); }

  /// Get number of references pointing here, too
  BOOLEAN count(leftv res) { return construct(res, m_data.count() - 1); }

  // Get internal indentifier
  BOOLEAN enumerate(leftv res) { return construct(res, (long)(data_type*)m_data); }

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

  /// Recover the actual object from raw Singular data
  static self cast(void* data) {
    assume(data != NULL);
    return self(static_cast<data_type*>(data));
  }

  /// Recover the actual object from Singular interpreter object
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
  /// Construct void-style object
  static BOOLEAN construct(leftv res) {
    res->data = NULL;
    res->rtyp = NONE;
    return FALSE;
  }


protected:
  /// Store pointer to actual data
  data_ptr m_data;
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
    return CountedRef::resolve(arg) || 
      CountedRef::cast(result).assign(result, arg);	
  }
  
  // Case: copy reference
  if (result->Typ() == arg->Typ())
    return CountedRef::cast(arg).outcast(result);

  // Case: new reference
  if ((arg->rtyp == IDHDL) || CountedRef::is_ref(arg))
    return CountedRef(arg).outcast(result);

  Werror("Can only take reference from identifier");
  return TRUE;
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
 
  if ((op == DEF_CMD) || (op == head->Typ())) {
    res->rtyp = head->Typ();
    return iiAssign(res, head);
  }

  if(op == LINK_CMD) {
    res->rtyp =  DEF_CMD;
    return CountedRef::cast(head).dereference(head) || iiAssign(res, head);
  }

  return CountedRef::cast(head).dereference(head) || iiExprArith1(res, head, op);
}

/// blackbox support - binary operations
BOOLEAN countedref_Op2(int op, leftv res, leftv head, leftv arg)
{
   return countedref_CheckInit(res, head) ||
    CountedRef::cast(head).dereference(head) ||
    CountedRef::resolve(arg) ||
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

  /// Reinterprete @c CountedRef as @c CountedRefShared
  CountedRefShared(const base& rhs):  base(rhs) { }

  /// Genreate   
  CountedRefShared(data_ptr rhs):  base(rhs) { }

public:
  /// Default constructor for initialized, but all-zero, shared data object
  CountedRefShared():  base(new data_type) { }

  /// Construct internal copy of Singular interpreter object
  explicit CountedRefShared(leftv arg):  base(new data_type(arg, data_type::copy_tag())) { }

  /// Construct new reference to internal data 
  CountedRefShared(const self& rhs): base(rhs) { }

  /// Desctruct 
  ~CountedRefShared() { }

  /// Change reference to shared data 
  self& operator=(const self& rhs) {
    return static_cast<self&>(base::operator=(rhs));
  }

  /// Recovering outcasted @c CountedRefShared object from interpreter object
  static self cast(leftv arg) { return base::cast(arg); }

  /// Recovering outcasted @c CountedRefShared object from raw data
  static self cast(void* arg) { return base::cast(arg); }

  /// Temporarily wrap with identifier for '[' and '.' operation
  self subscripted() { return self(m_data->subscripted()); }

  BOOLEAN retrieve(leftv res, int typ) { 
    return (m_data->retrieve(res) && outcast(res, typ));
  }
};

/// Blackbox support - generate initialized, but all-zero - shared data 
void* countedref_InitShared(blackbox*)
{
  return CountedRefShared().outcast();
}

/// blackbox support - binary operations
BOOLEAN countedref_Op2Shared(int op, leftv res, leftv head, leftv arg)
{
  if  ((op == '[') || (op == '.')) {
    if (countedref_CheckInit(res, head))  return TRUE;
    CountedRefShared indexed = CountedRefShared::cast(head).subscripted();

    int typ = head->Typ();
    return indexed.dereference(head) || CountedRefShared::resolve(arg) || 
      iiExprArith2(res, head, op, arg) || indexed.retrieve(res, typ);
  }

  return countedref_Op2(op, res, head, arg);
}

/// blackbox support - n-ary operations
BOOLEAN countedref_OpM(int op, leftv res, leftv args)
{
  if (args->Data() == NULL) return FALSE;

  if(op == SYSTEM_CMD) {
    if (args->next) {
      leftv next = args->next;
      args->next = NULL;

      char* name = (next->Typ() == STRING_CMD? 
                    (char*) next->Data(): (char*)next->Name());
      next = next->next;

      if (strcmp(name, "help") == 0) {
        PrintS("system(<ref>, ...): extended functionality for reference/shared data <ref>\n");
        PrintS("  system(<ref>, count)         - number of references pointing to <ref>\n");
        PrintS("  system(<ref>, enumerate)     - unique number for identifying <ref>\n");
        PrintS("  system(<ref>, undefined)     - checks whether <ref> had been assigned\n");
        PrintS("  system(<ref>, \"help\")        - prints this information message\n");
        PrintS("  system(<ref>, \"typeof\")      - actual type referenced by <ref>\n");
        PrintS("  system(<ref1>, same, <ref2>) - tests for identic reference objects\n");
        return CountedRef::construct(res);
      }
      if (strncmp(name, "undef", 5) == 0) {
	return CountedRef::construct(res, args->Data()? 
                          (CountedRef::cast(args).unassigned()? 1: 2): 0);
      }

      CountedRef obj = CountedRef::cast(args);
      if (next) {
        if (strcmp(name, "same") == 0) return obj.same(res, next);
        // likewise may be hard to interprete, so we not not document it above
        if (strncmp(name, "like", 4) == 0) return obj.likewise(res, next);  
      }
      if (strncmp(name, "count", 5) == 0) return obj.count(res);
      if (strncmp(name, "enum", 4) == 0) return obj.enumerate(res);
      if (strcmp(name, "name") == 0) return obj.name(res); // undecumented
      if (strncmp(name, "typ", 3) == 0) return obj.type(res);
    }
    return TRUE;
  }
  if (op == LIST_CMD){
    res->rtyp = op;
    return jjLIST_PL(res, args);
  }

  return CountedRef::cast(args).dereference(args) || iiExprArithM(res, args, op);
}

/// blackbox support - assign element
BOOLEAN countedref_AssignShared(leftv result, leftv arg)
{
  /// Case: replace assignment behind reference
  if ((result->Data() != NULL)  && !CountedRefShared::cast(result).unassigned()) {
    if (CountedRefShared::resolve(arg)) return TRUE;
    return CountedRefShared::cast(result).assign(result, arg);
    return FALSE;
  }
  
  /// Case: new reference to already shared data
  if (result->Typ() == arg->Typ()) {
    if (result->Data() != NULL) 
      CountedRefShared::cast(result).destruct();
    return CountedRefShared::cast(arg).outcast(result);
  }  
  if(CountedRefShared::cast(result).unassigned()) {
   // CountedRefShared::cast(result) = arg;
   return CountedRefShared::cast(result).assign(result, arg);

    return FALSE;
  }
    
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
  CountedRefEnv::ref_id()=setBlackboxStuff(bbx, "reference");

  /// The @c shared type is "inherited" from @c reference.
  /// It just uses another constructor (to make its own copy of the).
  blackbox *bbxshared = 
    (blackbox*)memcpy(omAlloc(sizeof(blackbox)), bbx, sizeof(blackbox));
  bbxshared->blackbox_Assign  = countedref_AssignShared;
  bbxshared->blackbox_destroy = countedref_destroyShared;
  bbxshared->blackbox_Op2     = countedref_Op2Shared;
  bbxshared->blackbox_Init    = countedref_InitShared;
  bbxshared->data             = omAlloc0(newstruct_desc_size());
  CountedRefEnv::sh_id()=setBlackboxStuff(bbxshared, "shared");
}

#ifdef HAVE_DYNAMIC_COUNTEDREF
extern "C" { void mod_init() { countedref_init(); } }
#endif

