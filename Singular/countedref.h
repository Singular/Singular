// -*- c++ -*-
//*****************************************************************************
/** @file countedref.h
 *
 * This file defines reusable classes supporting reference counted interpreter
 * objects and initiates the @c blackbox operations for high-level types
 * 'reference' and 'shared'.
 *
 * @note This works was supported by the "Industrial Algebra" project.
 *
 * @author Alexander Dreyer
 * @date 2012-08-15
 *
 * @par Copyright:
 *   (c) 2012 by The Singular Team, see LICENSE file
**/
//*****************************************************************************


#ifndef SINGULAR_COUNTEDREF_H_
#define SINGULAR_COUNTEDREF_H_

#include "omalloc/omalloc.h"
#include "kernel/structs.h"
#include "Singular/subexpr.h"
#include "Singular/idrec.h"
#include "Singular/ipid.h"
/** @class CountedRefPtr
 * This class implements a smart pointer which handles pointer-style access
 * to a reference-counted structure and destructing the latter after use.
 *
 * The template arguments, include the pointer type @c PtrType, and two
 * integral (bool) properties: use @c Nondestructive to disallow destruction
 * and @c NeverNull to assume, that @c PtrType cannot be @c NULL.
 * Finally, @c CountType allows you to select a typ to represent the internal reference count.
 *
 * @note The class of @c PtrType must have an accessible integral attribute @c ref.
 * For convenience use @c RefCounter as public base.
 * In addition you must overload @c void CountedRefPtr_kill(PtrType) accordingly.
 **/
template <class PtrType, bool Nondestructive = false, bool NeverNull = false,
          class CountType = short>
class CountedRefPtr {
  typedef CountedRefPtr self;

public:
  //{ @name Name template arguments
  typedef PtrType ptr_type;
  typedef CountType count_type;
  enum { nondestructive = Nondestructive, never_null = NeverNull };
  //}

  /// Default constructor @note: exisis only if @c NeverNull is false
  CountedRefPtr(): m_ptr(NULL) {}

  /// Convert from pointer
  CountedRefPtr(ptr_type ptr): m_ptr(ptr) { reclaim(); }

  /// Convert from compatible smart pointer
  template <bool Never>
  CountedRefPtr(const CountedRefPtr<ptr_type, !nondestructive, Never, count_type>& rhs):
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

  /// @name Reference count interface
  //@{
  count_type count() const { return (*this? m_ptr->ref: 0); }
  void reclaim() { if (*this) ++m_ptr->ref; }
  void release() {
    if (*this && (--m_ptr->ref <= 0) && !nondestructive)
      CountedRefPtr_kill(m_ptr);
  }
  //@}

private:
  /// Store actual pointer
  ptr_type m_ptr;
};

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


template <class PtrType>
class CountedRefWeakPtr;

template <class PtrType>
class CountedRefIndirectPtr:
  public RefCounter {
public:
  friend class CountedRefWeakPtr<PtrType>;
  ~CountedRefIndirectPtr()  { }

private:
  CountedRefIndirectPtr(PtrType ptr): m_ptr(ptr) { }
  CountedRefIndirectPtr& operator=(PtrType ptr) { m_ptr = ptr; return *this; }

  PtrType m_ptr;
};

template <class PtrType>
inline void CountedRefPtr_kill(CountedRefIndirectPtr<PtrType>* pval) { delete pval; }

template <class PtrType>
class CountedRefWeakPtr {
  typedef CountedRefWeakPtr self;

public:

  /// @name Name template arguments
  //@{ Name template arguments
  typedef PtrType ptr_type;
  typedef CountedRefPtr<CountedRefIndirectPtr<ptr_type>*> ptrptr_type;
  //@}

  /// Construct unassigned weak reference
  CountedRefWeakPtr(): m_indirect(NULL) { }

  /// Convert from pointer
  CountedRefWeakPtr(ptr_type ptr): m_indirect(new CountedRefIndirectPtr<ptr_type>(ptr)) { }

  /// Construct copy
  CountedRefWeakPtr(const self& rhs):  m_indirect(rhs.m_indirect) { }

  /// Unlink one reference (handled by CountedRefPtr)
  ~CountedRefWeakPtr() { }

  /// Mark weak reference as invalid
  void invalidate() {  *this = NULL; }

  /// Test whether reference was never used
  bool unassigned() const { return !m_indirect; }
  /// Pointer-style interface
  //@{
  operator bool() const {  return operator->(); }
  self& operator=(const self& rhs) {
    m_indirect = rhs.m_indirect;
    return *this;
  }
  self& operator=(ptr_type ptr) {
    if (!m_indirect)
      m_indirect = new CountedRefIndirectPtr<ptr_type>(ptr);
    else
      m_indirect->m_ptr = ptr;
    return *this;
  }
  bool operator==(ptr_type ptr) const {
    return m_indirect &&(m_indirect->m_ptr == ptr);
  }
  bool operator!=(ptr_type rhs) const { return !operator==(rhs); }
  const ptr_type operator->() const { return (m_indirect? m_indirect->m_ptr: NULL); }
  ptr_type operator->() {   return (m_indirect? m_indirect->m_ptr:NULL); }
  //@}
private:
  ptrptr_type m_indirect;
};



/** @class LeftvHelper
 * This class implements some recurrent code sniplets to be used with
 * @c leftv and @c idhdl.implements a refernce counter which we can use
 **/
class LeftvHelper {
public:
  static leftv idify(leftv head, idhdl* root) {
    idhdl handle = newid(head, root);
    leftv res = (leftv)omAlloc0(sizeof(*res));
    res->data =(void*) handle;
    res->rtyp = IDHDL;
    return res;
  }

  static idhdl newid(leftv head, idhdl* root) {

    THREAD_VAR static unsigned int counter = 0;
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

/** @class LeftvShallow
 * Ths class wraps @c leftv by taking into acount memory allocation, destruction
 * as well as shallowly copying of a given @c leftv, i.e. we just copy auxiliary
 * information (like subexpressions), but not the actual data.
 *
 * @note This is useful to avoid invalidating @c leftv while operating on th
 **/
class LeftvShallow:
  public LeftvHelper {
  typedef LeftvShallow self;

public:
  /// Just allocate (all-zero) @c leftv
  LeftvShallow(): m_data(allocate()) { }
  /// Shallow copy the input data
  LeftvShallow(leftv data): m_data(shallowcpy(data)) { }
  /// Construct (shallow) copy of @c *this
  LeftvShallow(const self& rhs):  m_data(shallowcpy(rhs.m_data)) { }

  /// Destruct
  ~LeftvShallow() {
    recursivekill(m_data->e);
    omFree(m_data);
  }

  /// Assign shallow copy of the input
  self& operator=(leftv rhs) {
    recursivekill(m_data->e);
    shallowcpy(m_data, rhs);
    return *this;
  }
  /// Assign (shallow) copy of @c *this
  self& operator=(const self& rhs) { return (*this) = rhs.m_data; }

  /// @name Pointer-style access
  //@{
  /*const*/ leftv operator->() const { return m_data;  }
  /*^ warning: 'const' type qualifier on return type has no effect!!! */
  leftv operator->() { return m_data;  }
  //@]

protected:
  /// The actual data pointer
  leftv m_data;
};

/** @class LeftvDeep
 * This class wraps @c leftv by taking into acount memory allocation, destruction
 * as well as deeply copying of a given @c leftv, i.e. we also take over
 * ownership of the @c leftv data.
 *
 * We have two variants:
   + LeftvDeep(leftv):           treats referenced identifiers as "the data"
   + LeftvDeep(leftv, copy_tag): takes care of a full copy of identifier's data
 *
 * @note It invalidats @c leftv on input.
 **/
class LeftvDeep:
  public LeftvHelper {
  typedef LeftvDeep self;

  /// @name Do not permit copying (avoid inconsistence)
  //@{
  self& operator=(const self&);
  LeftvDeep(const self&);
  //@}

public:
  /// Allocate all-zero object by default
  LeftvDeep(): m_data(allocate()) {}

  /// Store a deep copy of the data
  /// @ note Occupies the provided @c leftv and invalidates the latter
  LeftvDeep(leftv data): m_data(cpy(data)) {
    data->e = NULL;   // occupy subexpression
    if(!isid()) m_data->data=data->CopyD();
  }

  /// Construct even deeper copy:
  /// Skip identifier (if any) and take care of the data on our own
  struct copy_tag {};
  LeftvDeep(leftv data, copy_tag): m_data(allocate()) {  m_data->Copy(data);  }

  /// Really clear data
  ~LeftvDeep() { m_data->CleanUp(); }

  /// @name Access via shallow copy to avoid invalidating the stored handle
  //@{
  operator LeftvShallow() { return m_data; }
  LeftvShallow operator*() {return *this; }
  //@}

  /// Determine whether we point to the same data
  bool like(const self& rhs) const { return m_data->data == rhs.m_data->data; }

  /// Reassign a new deep copy by occupieing another @c leftv
  /// @note clears @c *this in the first
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

    /// @note @c attrib should read the attributes of the identifier
    if (isid()) {
      result->attribute = ((idhdl)m_data->data)->attribute;
      result->flag = ((idhdl)m_data->data)->flag;

    }
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


  /// Check for being an identifier
  BOOLEAN isid() const { return m_data->rtyp==IDHDL;}
  /// Test whether we reference to ring-dependent data
  BOOLEAN ringed() { return m_data->RingDependend(); }
  /// Check whether (all-zero) initialized data was never assigned.
  BOOLEAN unassigned() const { return m_data->Typ()==0; }

  /// Wrap data by identifier, if not done yet
  leftv idify(idhdl* root) {
    leftv res = (isid()? m_data: LeftvHelper::idify(m_data, root));
    ++(((idhdl)res->data)->ref);
    return res;
  }

  /// Erase identifier handles by @c *this
  /// @note Assumes that we reference an identifier and that we own the latter.
  /// This is useful to clear the result of a subsequent call of @c idify.
  void clearid(idhdl* root) {
    assume(isid());
    if (--((idhdl)m_data->data)->ref <= 0)  // clear only if we own
      LeftvHelper::clearid((idhdl)m_data->data, root);
  }

private:
  /// Store the actual data
  leftv m_data;
};

/// Initialize @c blackbox types 'reference' and 'shared', or both
void countedref_reference_load();
void countedref_shared_load();

inline void
countedref_init()
{
  countedref_reference_load();
  countedref_shared_load();
}


#endif /*SINGULAR_COUNTEDREF_H_ */

