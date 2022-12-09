// -*- c++ -*-
//*****************************************************************************
/** @file countedref.cc
 *
 * @author Alexander Dreyer
 * @date 2012-08-15
 *
 * This file defines reference countes interpreter objects and adds the
 * @c blackbox operations for high-level types 'reference' and 'shared'.
 *
 * @note This works was supported by the "Industrial Algebra" project.
 *
 * @par Copyright:
 *   (c) 2012 by The Singular Team, see LICENSE file
**/
//*****************************************************************************





#include "kernel/mod2.h"

#include "ipid.h"

#include "countedref.h"

#include "blackbox.h"
#include "newstruct.h"
#include "ipshell.h"


/// Overloading ring destruction
inline void CountedRefPtr_kill(ring r) { rKill(r); }


/** @class CountedRefData
 * This class stores a reference counter as well as a Singular interpreter object.
 * It also take care of the context, e.g. the current ring, wrap object, etc.
 **/
class CountedRefData:
  public RefCounter {
  typedef CountedRefData self;
public:
  typedef CountedRefWeakPtr<self*> back_ptr;
private:
  typedef RefCounter base;

  /// Generate object linked to other reference (e.g. for subscripts)
  CountedRefData(leftv wrapid, back_ptr back):
    base(), m_data(wrapid), m_ring(back->m_ring), m_back(back) {
  }

  /// @name Disallow copying to avoid inconsistence
  //@{
  self& operator=(const self&);
  CountedRefData(const self&);
  //@}

public:
  typedef LeftvDeep::copy_tag copy_tag;

  /// Fix smart pointer type to referenced data
  typedef back_ptr::ptr_type ptr_type;

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
  ~CountedRefData() {
    if (!m_back.unassigned()) {
      if (m_back == this)
        m_back.invalidate();
      else
        m_data.clearid(root());
    }
  }

  /// Generate object for indexing
  ptr_type wrapid() { return new self(m_data.idify(root()), weakref()); }

  /// Gerenate  weak (but managed) reference to @c *this
  back_ptr weakref() {
    if (m_back.unassigned())
      m_back = this;
    return m_back;
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
    return (m_back && (m_back != this) && m_back->rering());
  }

  /// Get the current context
  idhdl* root() { return  (m_ring? &m_ring->idroot: &IDROOT); }

  /// Check whether identifier became invalid
  BOOLEAN broken() const {
    if (!m_back.unassigned() && !m_back)
      return complain("Back-reference broken");

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
  BOOLEAN complain(const char* text) const
  {
    WerrorS(text);
    return TRUE;
  }

  /// Store ring for ring-dependent objects
  static ring parent(leftv rhs)
  {
    return (rhs->RingDependend()? currRing: NULL);
  }

protected:
  /// Singular object
  LeftvDeep m_data;

  /// Store namespace for ring-dependent objects
  ring_ptr m_ring;

  /// Reference to actual object for wrap structures
  back_ptr m_back;
};

/// Supporting smart pointer @c CountedRefPtr
inline void CountedRefPtr_kill(CountedRefData* data) { delete data; }


/// blackbox support - initialization
void* countedref_Init(blackbox*)
{
  return NULL;
}

/// We use the function pointer as a marker of reference types
/// for CountedRef::is_ref(leftv), see the latter for details
BOOLEAN countedref_CheckAssign(blackbox */*b*/, leftv /*L*/, leftv /*R*/)
{
  return FALSE;
}


class CountedRef {
  typedef CountedRef self;

public:
  /// name type for identifiers
  typedef int id_type;

  /// Name type for handling referenced data
  typedef CountedRefData data_type;

  /// Fix smart pointer type to referenced data
  typedef CountedRefPtr<CountedRefData*> data_ptr;

  /// Check whether argument is already a reference type
  /// @note We check for the function pointer @c countedref_CheckAssign here,
  /// that we (ab-)use as a unique marker. This avoids to check a bunch of
  /// of runtime-varying @c typ IDs for identifying reference-like types.
  static BOOLEAN is_ref(leftv arg) {
    int typ = arg->Typ();
    return ((typ > MAX_TOK) &&
            (getBlackboxStuff(typ)->blackbox_CheckAssign == countedref_CheckAssign));
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

  // Get internal identifier
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
    assume(arg != NULL); assume(is_ref(arg));
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
void countedref_Print(blackbox */*b*/, void* ptr)
{
  if (ptr) (*CountedRef::cast(ptr))->Print();
  else PrintS("<unassigned reference or shared memory>");
}

/// blackbox support - convert to string representation
char* countedref_String(blackbox */*b*/, void* ptr)
{
  if (ptr == NULL) return omStrDup(sNoName_fe);
  return (*CountedRef::cast(ptr))->String();
}

/// blackbox support - copy element
void* countedref_Copy(blackbox*/*b*/, void* ptr)
{
  if (ptr) return CountedRef::cast(ptr).outcast();
  return NULL;
}

/// blackbox support - assign element
BOOLEAN countedref_Assign(leftv result, leftv arg)
{
  // Case: replace assignment behind reference
  if (result->Data() != NULL) {
    CountedRef ref = CountedRef::cast(result);
    return CountedRef::resolve(arg) || ref.assign(result, arg);
  }

  // Case: copy reference
  if (result->Typ() == arg->Typ())
    return CountedRef::cast(arg).outcast(result);

  // Case: new reference
  if ((arg->rtyp == IDHDL) || CountedRef::is_ref(arg))
    return CountedRef(arg).outcast(result);

  WerrorS("Can only take reference from identifier");
  return TRUE;
}

BOOLEAN countedref_CheckInit(leftv res, leftv arg)
{
  if (arg->Data() != NULL) return FALSE;
  res->rtyp = NONE;
  WerrorS("Noninitialized access");
  return TRUE;
}

/// blackbox support - unary operations
BOOLEAN countedref_Op1(int op, leftv res, leftv head)
{
  if(op == TYPEOF_CMD)
    return blackboxDefaultOp1(op, res, head);

  if (countedref_CheckInit(res, head)) return TRUE;

  if ((op == DEF_CMD) || (op == head->Typ()))
  {
    res->rtyp = head->Typ();
    return iiAssign(res, head);
  }

  CountedRef ref = CountedRef::cast(head);
  return ref.dereference(head) ||
    iiExprArith1(res, head, op == LINK_CMD? head->Typ(): op);
}



/// blackbox support - binary operations (resolve seocnd argument)
static BOOLEAN countedref_Op2_(int op, leftv res, leftv head, leftv arg)
{
  if (CountedRef::is_ref(arg))
  {
    CountedRef ref = CountedRef::cast(arg);
    return ref.dereference(arg) || iiExprArith2(res, head, op, arg);
  }
  return  iiExprArith2(res, head, op, arg);
}

BOOLEAN countedref_Op2(int op, leftv res, leftv head, leftv arg)
{
  if (countedref_CheckInit(res, head)) return TRUE;
  if (CountedRef::is_ref(head))
  {
    CountedRef ref = CountedRef::cast(head);
    return ref.dereference(head) || countedref_Op2_(op, res, head, arg);
  }
  return countedref_Op2_(op, res, head, arg);
}

static BOOLEAN countedref_Op3__(int op, leftv res, leftv head, leftv arg1, leftv arg2)
{

  if (CountedRef::is_ref(arg2))
  {
    CountedRef ref = CountedRef::cast(arg2);
    return ref.dereference(arg2) || iiExprArith3(res, op, head, arg1, arg2);
  }
  return iiExprArith3(res, op, head, arg1, arg2);
}

static BOOLEAN countedref_Op3_(int op, leftv res, leftv head, leftv arg1, leftv arg2)
{
  if (CountedRef::is_ref(arg1))
  {
    CountedRef ref = CountedRef::cast(arg1);
    return ref.dereference(arg1) || countedref_Op3__(op, res, head, arg1, arg2);
  }
  return countedref_Op3__(op, res, head, arg1, arg2);
}


/// blackbox support - ternary operations
BOOLEAN countedref_Op3(int op, leftv res, leftv head, leftv arg1, leftv arg2)
{
  if (countedref_CheckInit(res, head)) return TRUE;
  if (CountedRef::is_ref(head))
  {
    CountedRef ref = CountedRef::cast(head);
    return ref.dereference(head) || countedref_Op3_(op, res, head, arg1, arg2);
  }
  return countedref_Op3_(op, res, head, arg1, arg2);
}


/// blackbox support - destruction
void countedref_destroy(blackbox */*b*/, void* ptr)
{
  if (ptr) CountedRef::cast(ptr).destruct();
}


class CountedRefShared:
  public CountedRef
  {
  typedef CountedRefShared self;
  typedef CountedRef base;

  /// Reinterprete @c CountedRef as @c CountedRefShared
  CountedRefShared(const base& rhs):  base(rhs) { }

  /// Generate from data pointer
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
  self wrapid() { return self(m_data->wrapid()); }

  /// Generate weak reference (may get invalid)
  data_type::back_ptr weakref() { return m_data->weakref(); }

  /// Recover more information (e.g. subexpression data) from computed result
  BOOLEAN retrieve(leftv res, int typ)
  {
    return (m_data->retrieve(res) && outcast(res, typ));
  }
};

/// Blackbox support - generate initialized, but all-zero - shared data
void* countedref_InitShared(blackbox*)
{
  return CountedRefShared().outcast();
}

/// Blackbox support - unary operation for shared data
BOOLEAN countedref_Op1Shared(int op, leftv res, leftv head)
{
  if(op == TYPEOF_CMD)
    return blackboxDefaultOp1(op, res, head);

  if (countedref_CheckInit(res, head)) return TRUE;

  if ((op == DEF_CMD) || (op == head->Typ()))
  {
    res->rtyp = head->Typ();
    return iiAssign(res, head);
  }

  CountedRefShared ref = CountedRefShared::cast(head);

  if (op == LINK_CMD)
  {
    if (ref.dereference(head)) return TRUE;
    res->Copy(head);
    return (res->Typ() == NONE);
  }

  CountedRefShared wrap = ref.wrapid();
  int typ = head->Typ();
  return wrap.dereference(head) || iiExprArith1(res, head, op) ||
    wrap.retrieve(res, typ);
}


/// blackbox support - binary operations
BOOLEAN countedref_Op2Shared(int op, leftv res, leftv head, leftv arg)
{
  if (countedref_CheckInit(res, head))  return TRUE;

  if (CountedRefShared::is_ref(head))
  {
    CountedRefShared wrap = CountedRefShared::cast(head).wrapid();
    int typ = head->Typ();
    return wrap.dereference(head) || countedref_Op2_(op, res, head, arg) ||
      wrap.retrieve(res, typ);
  }

  return countedref_Op2_(op, res, head, arg);
}

/// blackbox support - n-ary operations
BOOLEAN countedref_OpM(int op, leftv res, leftv args)
{
  if (args->Data() == NULL) return FALSE;

  if(op == SYSTEM_CMD)
  {
    if (args->next)
    {
      leftv next = args->next;
      args->next = NULL;

      char* name = (next->Typ() == STRING_CMD?
                    (char*) next->Data(): (char*)next->Name());
      next = next->next;

      if (strcmp(name, "help") == 0)
      {
        PrintS("system(<ref>, ...): extended functionality for reference/shared data <ref>\n");
        PrintS("  system(<ref>, count)         - number of references pointing to <ref>\n");
        PrintS("  system(<ref>, enumerate)     - unique number for identifying <ref>\n");
        PrintS("  system(<ref>, undefined)     - checks whether <ref> had been assigned\n");
        PrintS("  system(<ref>, \"help\")        - prints this information message\n");
        PrintS("  system(<ref>, \"typeof\")      - actual type referenced by <ref>\n");
        PrintS("  system(<ref1>, same, <ref2>) - tests for identic reference objects\n");
        return CountedRef::construct(res);
      }
      if (strncmp(name, "undef", 5) == 0)
      {
        return CountedRef::construct(res, args->Data()?
                          (CountedRef::cast(args).unassigned()? 1: 2): 0);
      }

      CountedRef obj = CountedRef::cast(args);
      if (next)
      {
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
  if (op == LIST_CMD)
  {
    res->rtyp = op;
    return jjLIST_PL(res, args);
  }
  CountedRef ref = CountedRef::cast(args);
  return ref.dereference(args) || iiExprArithM(res, args, op);
}

/// blackbox support - assign element
BOOLEAN countedref_AssignShared(leftv result, leftv arg)
{
  /// Case: replace assignment behind reference
  if ((result->Data() != NULL)  && !CountedRefShared::cast(result).unassigned()) {
    CountedRef ref = CountedRef::cast(result);
    return CountedRef::resolve(arg) || ref.assign(result, arg);
  }

  /// Case: new reference to already shared data
  if (result->Typ() == arg->Typ())
  {
    if (result->Data() != NULL)
      CountedRefShared::cast(result).destruct();
    return CountedRefShared::cast(arg).outcast(result);
  }
  if(CountedRefShared::cast(result).unassigned())
  {
    return CountedRefShared::cast(result).assign(result, arg);
  }

  /// Case: new shared data
  return CountedRefShared(arg).outcast(result);
}

/// blackbox support - destruction
void countedref_destroyShared(blackbox */*b*/, void* ptr)
{
  if (ptr) CountedRefShared::cast(ptr).destruct();
}


BOOLEAN countedref_serialize(blackbox */*b*/, void *d, si_link f)
{
  sleftv l;
  memset(&l,0,sizeof(l));
  l.rtyp = STRING_CMD;
  l.data = (void*)omStrDup("shared"); // references are converted
  f->m->Write(f, &l);
  CountedRefShared::cast(d).dereference(&l);
  f->m->Write(f, &l);
  return FALSE;
}

BOOLEAN countedref_deserialize(blackbox **/*b*/, void **d, si_link f)
{
  // rtyp must be set correctly (to the blackbox id) by routine calling
  leftv data=f->m->Read(f);
  CountedRefShared sh(data);
  *d = sh.outcast();
  return FALSE;
}

void countedref_reference_load()
{
  int tok;
  if (blackboxIsCmd("reference", tok) == ROOT_DECL)
    return;

  blackbox *bbx = (blackbox*)omAlloc0(sizeof(blackbox));
  bbx->blackbox_CheckAssign = countedref_CheckAssign;
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
  bbx->blackbox_serialize   = countedref_serialize;
  bbx->blackbox_deserialize = countedref_deserialize;
  bbx->data             = omAlloc0(newstruct_desc_size());
  setBlackboxStuff(bbx, "reference");
}

void countedref_shared_load()
{
  int tok;
  if (blackboxIsCmd("shared", tok) == ROOT_DECL)
    return;

  blackbox *bbxshared = (blackbox*)omAlloc0(sizeof(blackbox));
  bbxshared->blackbox_String  = countedref_String;
  bbxshared->blackbox_Print  = countedref_Print;
  bbxshared->blackbox_Copy    = countedref_Copy;
  bbxshared->blackbox_Op3     = countedref_Op3;
  bbxshared->blackbox_OpM     = countedref_OpM;
  bbxshared->blackbox_serialize   = countedref_serialize;
  bbxshared->blackbox_deserialize = countedref_deserialize;

  bbxshared->blackbox_CheckAssign = countedref_CheckAssign;
  bbxshared->blackbox_Assign  = countedref_AssignShared;
  bbxshared->blackbox_destroy = countedref_destroyShared;
  bbxshared->blackbox_Op1     = countedref_Op1Shared;
  bbxshared->blackbox_Op2     = countedref_Op2Shared;
  bbxshared->blackbox_Init    = countedref_InitShared;
  bbxshared->data             = omAlloc0(newstruct_desc_size());
  setBlackboxStuff(bbxshared, "shared");
}


