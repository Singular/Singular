/// -*- c++ -*-
//*****************************************************************************
/** @file countedref.h
 *
 * @author Alexander Dreyer
 * @date 2012-08-15
 *
 * This file defines reusable classes supporting reference counted interpreter
 * objects and initiates the @c blackbox operations for high-level types
 * 'reference' and 'shared'.
 *
 * @note This works was supported by the "Industrial Algebra" project.
 * 
 * @par Copyright:
 *   (c) 2012 by The Singular Team, see LICENSE file
**/
//*****************************************************************************


#ifndef SINGULAR_COUNTEDREF_H_
#define SINGULAR_COUNTEDREF_H_

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
#if 0
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
#endif
/// Overloading ring destruction
inline void CountedRefPtr_kill(ring r) { rKill(r); }

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
    leftv res = (isid()? m_data: LeftvHelper::idify(m_data, root));
    ++(((idhdl)res->data)->ref);
    return res;
  }


  void clearid(idhdl* root) {
    assume(isid());
    if (--((idhdl)m_data->data)->ref <= 0)  // clear only if we own
      LeftvHelper::clearid((idhdl)m_data->data, root);
  }

private:
  leftv m_data;
};

/// Initialize @c blackbox types 'reference' and 'shared'
void countedref_init();


#endif /*SINGULAR_COUNTEDREF_H_ */

