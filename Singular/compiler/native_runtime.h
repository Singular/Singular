#ifndef SINGULAR_NATIVE_COMPILER_RUNTIME_H
#define SINGULAR_NATIVE_COMPILER_RUNTIME_H

// Helpers for generated C++, not an expression/statement interpreter.
#include "Singular/libsingular.h"
#include "Singular/htable.h"
#include "Singular/lists.h"
#include "Singular/maps_ip.h"
#include "Singular/blackbox.h"
#include "polys/weight.h"
#include "polys/sbuckets.h"
#include "polys/ext_fields/transext.h"
#include <climits>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>
#include <string>
#include <initializer_list>

// Exported direct newstruct kernel; the public header keeps descriptors opaque.
BOOLEAN newstruct_Op2(int, leftv, leftv, leftv);

namespace snative {
class SourceProc {
  procinfov value;
public:
  explicit SourceProc(procinfov source=nullptr)
    : value(source==nullptr ? nullptr : piCopy(source)) {}
  ~SourceProc() { if (value!=nullptr) piKill(value); }
  SourceProc(const SourceProc &)=delete;
  SourceProc &operator=(const SourceProc &)=delete;
  SourceProc(SourceProc &&source) noexcept: value(source.release()) {}
  SourceProc &operator=(SourceProc &&source) noexcept {
    if (this!=&source) {
      procinfov replacement=source.release();
      if (value!=nullptr) piKill(value);
      value=replacement;
    }
    return *this;
  }
  procinfov get() const { return value; }
  procinfov release() { procinfov source=value; value=nullptr; return source; }
};

inline package source_package(const char *library) {
  char *package_name=iiConvName(library);
  idhdl package_id=basePack->idroot->get(package_name,0);
  if (package_id==nullptr) {
    if (iiLibCmd(library,FALSE,TRUE,FALSE)) {
      omFreeBinAddr(package_name);
      throw std::runtime_error("cannot load interpreted source library");
    }
    package_id=basePack->idroot->get(package_name,0);
  }
  omFreeBinAddr(package_name);
  // iiLibCmd's already-loaded fast path opens but does not close a FILE.
  // Resolve an existing package ourselves, without touching the interpreter.
  if (package_id==nullptr || IDTYP(package_id)!=PACKAGE_CMD ||
      IDPACKAGE(package_id)->language<LANG_SINGULAR || !IDPACKAGE(package_id)->loaded)
    throw std::runtime_error("interpreted source package is unavailable");
  return IDPACKAGE(package_id);
}

inline procinfov interpreted_source(procinfov source) {
  // A later native module may be loaded after an earlier one rebound the same
  // library procedure. Follow only compiler-installed source links and reject
  // ordinary C procedures, cycles, and unreasonably deep/corrupt chains.
  procinfov seen[32];
  int seen_count=0;
  while (source!=nullptr && source->language==LANG_C) {
    if (seen_count==(int)(sizeof(seen)/sizeof(seen[0])))
      throw std::runtime_error("interpreted source chain is too deep");
    for (int i=0;i<seen_count;++i)
      if (seen[i]==source)
        throw std::runtime_error("interpreted source chain is cyclic");
    seen[seen_count++]=source;
    source=source->data.o.singular_source;
  }
  if (source==nullptr || source->language!=LANG_SINGULAR)
    throw std::runtime_error("procedure has no interpreted source metadata");
  return source;
}

inline procinfov source_procedure(const char *library, const char *name,
                                  bool require_public) {
  package source_pack=source_package(library);
  // Some .lib files also load a same-named .so (e.g. customstd.lib). Such
  // packages are LANG_C/LANG_MIX but still contain source procedures.
  idhdl source_id=source_pack->idroot->get(name,0);
  if (source_id==nullptr || IDTYP(source_id)!=PROC_CMD)
    throw std::runtime_error("interpreted source procedure is unavailable");
  procinfov source=interpreted_source(IDPROC(source_id));
  if (require_public && source->is_static)
    throw std::runtime_error("interpreted alias source must be a public procedure");
  return source;
}

// Fallbacks are actual Singular procedure aliases, not native wrappers. They
// retain the interpreter's full calling convention and original package scope.
inline procinfov alias_source(const char *library, const char *name) {
  return source_procedure(library,name,true);
}
inline SourceProc native_source(const char *library, const char *name) {
  // Explicit roots may be static, so unlike fallback aliases this resolver
  // deliberately does not impose public visibility.
  return SourceProc(source_procedure(library,name,false));
}
inline void alias_into(package target, const char *name, procinfov source) {
  idhdl h=target->idroot->get(name,0);
  if (h!=nullptr && IDTYP(h)!=PROC_CMD)
    throw std::runtime_error("compiled export name conflicts with a non-procedure");
  if (h==nullptr) h=enterid(name,0,PROC_CMD,&target->idroot,TRUE);
  if (h==nullptr) throw std::runtime_error("cannot create interpreted alias");
  procinfov copy=piCopy(source);
  piKill(IDPROC(h));
  IDPROC(h)=copy;
}
inline void install_alias(const char *name, procinfov source) {
  alias_into(currPack,name,source);
  if (currPack!=basePack) alias_into(basePack,name,source);
  Warn("%s is an interpreted alias, not native C++",name);
}
inline void check_export(const char *name) {
  package targets[2]={currPack,basePack};
  for (int i=0;i<2;++i) {
    idhdl h=targets[i]->idroot->get(name,0);
    if (h!=nullptr && IDTYP(h)!=PROC_CMD)
      throw std::runtime_error("compiled export name conflicts with a non-procedure");
  }
}
inline void prepare_native(const char *name) {
  // iiAddCproc mutates an existing procinfo in place. Detach prefixed bindings
  // first: a previous c_ alias may share that procinfo with the original!
  package targets[2]={currPack,basePack};
  for (int i=0;i<2;++i) {
    if (i==1 && currPack==basePack) break;
    idhdl h=targets[i]->idroot->get(name,0);
    if (h!=nullptr) {
      if (IDTYP(h)!=PROC_CMD)
        throw std::runtime_error("compiled export name conflicts with a non-procedure");
      procinfov fresh=(procinfov)idrecDataInit(PROC_CMD);
      piKill(IDPROC(h));
      IDPROC(h)=fresh;
    }
  }
}
inline void attach_native_source(const char *name,
                                 BOOLEAN (*function)(leftv,leftv),
                                 const SourceProc &source_ref) {
  procinfov source=source_ref.get();
  if (source==nullptr || source->language!=LANG_SINGULAR)
    throw std::runtime_error("native entry source is not an interpreted procedure");
  package targets[2]={currPack,basePack};
  procinfov attached[2]={nullptr,nullptr};
  int attached_count=0;
  for (int i=0;i<2;++i) {
    if (i==1 && targets[1]==targets[0]) break;
    idhdl h=targets[i]->idroot->get(name,0);
    if (h==nullptr || IDTYP(h)!=PROC_CMD) continue;
    procinfov target=IDPROC(h);
    if (target==nullptr || target->language!=LANG_C ||
        target->data.o.function!=function)
      continue;
    bool duplicate=false;
    for (int j=0;j<attached_count;++j)
      if (attached[j]==target) duplicate=true;
    if (duplicate) continue;
    // Retain before releasing an earlier mapping: source may be the same
    // procinfo and rebinding must never create a transient dangling pointer.
    procinfov retained=piCopy(source);
    procinfov previous=target->data.o.singular_source;
    target->data.o.singular_source=retained;
    if (previous!=nullptr && previous!=target) piKill(previous);
    attached[attached_count++]=target;
  }
  if (attached_count==0)
    throw std::runtime_error("cannot find the registered native entry for source metadata");
}

struct Context {
  ring r;
  int depth;
  unsigned long iterations;
  explicit Context(ring R): r(R), depth(0), iterations(0) {}
  void tick() {
    if (((++iterations & 1023UL) == 0) && siCntrlc)
      throw std::runtime_error("native procedure interrupted");
  }
};
struct Call {
  Context &ctx;
  explicit Call(Context &c): ctx(c) {
    // iplib.cc's nesting bound is private to that translation unit.
    if (ctx.depth + myynest >= (SIZEOF_LONG==8 ? 500 : 1000))
      throw std::runtime_error("native procedure nesting limit reached");
    ++ctx.depth;
  }
  ~Call() { --ctx.depth; }
};

// Owned values are move-only. Raw poly parameters are borrowed views; callers
// copy only when a destructive kernel needs ownership, never at a native call.
class Poly {
  poly value;
  ring owner;
public:
  Poly(poly p, ring r): value(p), owner(r) {}
  ~Poly() { p_Delete(&value, owner); }
  Poly(const Poly &) = delete;
  Poly &operator=(const Poly &) = delete;
  Poly(Poly &&p) noexcept: value(p.release()), owner(p.owner) {}
  Poly &operator=(Poly &&p) noexcept {
    if (this!=&p) { p_Delete(&value, owner); owner=p.owner; value=p.release(); }
    return *this;
  }
  static Poly copy(poly p, ring r) { return Poly(p_Copy(p,r),r); }
  poly get() const { return value; }
  ring base() const { return owner; }
  poly release() { poly p=value; value=nullptr; return p; }
};

// A typed polynomial formal is an assignment boundary.  With qringNF active,
// jiA_POLY normalizes an unflagged input modulo the quotient ideal.  Native
// code does not expose polynomial leftv flags internally, so materialize a
// private normal form at every typed call boundary.  Re-normalizing a value
// which was already marked FLAG_QRING is value-preserving, and, importantly,
// the caller's polynomial is never consumed or changed.
class PolyBinding {
  Poly owned;
  poly value;
public:
  PolyBinding(poly input, ring r): owned(nullptr,r), value(input) {
    if (input!=nullptr && TEST_V_QRING && r->qideal!=nullptr) {
      poly copy=p_Copy(input,r);
      p_Normalize(copy,r);
      copy=jj_NormalizeQRingP(copy,r);
      owned=Poly(copy,r);
      if (errorreported)
        throw std::runtime_error("native qring polynomial normalization failed");
      value=owned.get();
    }
  }
  PolyBinding(const PolyBinding &)=delete;
  PolyBinding &operator=(const PolyBinding &)=delete;
  poly get() const { return value; }
};

// Coefficients use the basering's coefficient domain, including transcendental
// extensions. Native-to-native parameters are borrowed; results own storage.
class Number {
  number value;
  coeffs owner;
public:
  Number(number n, coeffs cf): value(n), owner(cf) {}
  ~Number() { if (value!=nullptr) n_Delete(&value,owner); }
  Number(const Number &) = delete;
  Number &operator=(const Number &) = delete;
  Number(Number &&n) noexcept: value(n.release()), owner(n.owner) {}
  Number &operator=(Number &&n) noexcept {
    if (this!=&n) {
      if (value!=nullptr) n_Delete(&value,owner);
      owner=n.owner; value=n.release();
    }
    return *this;
  }
  static Number copy(number n, ring r) { return Number(n_Copy(n,r->cf),r->cf); }
  number get() const { return value; }
  coeffs base() const { return owner; }
  number release() { number n=value; value=nullptr; return n; }
};

// Interpreter strings are omalloc-owned NUL-terminated byte strings.  Keep
// that representation at the native ABI: std::string storage must never be
// handed to sleftv::CleanUp or returned through a STRING_CMD result.
class String {
  char *value;
public:
  explicit String(char *s): value(s) {}
  ~String() { if (value!=nullptr) omFree((ADDRESS)value); }
  String(const String &) = delete;
  String &operator=(const String &) = delete;
  String(String &&s) noexcept: value(s.release()) {}
  String &operator=(String &&s) noexcept {
    if (this!=&s) {
      if (value!=nullptr) omFree((ADDRESS)value);
      value=s.release();
    }
    return *this;
  }
  static String copy(const char *s) { return String(omStrDup(s==nullptr ? "" : s)); }
  const char *get() const { return value==nullptr ? "" : value; }
  char *write() { return value; }
  char *release() { char *s=value; value=nullptr; return s; }
};

// Ideals own both their generator array and every polynomial in it.  Retain
// the originating ring for destruction just like Poly does; native calls may
// change currRing after an owned result has crossed an ABI boundary.
class IdealView {
  ideal value;
  BITSET flag;
  // NUMBER_CMD reaches an ideal formal through NUMBER->MATRIX->IDEAL.  The
  // matrix assignment has already happened in ideal_from_number, so exactly
  // the next typed boundary must not re-run jiA_IDEAL's one-column STD rule.
  bool skip_next_binding;
public:
  IdealView(ideal i, BITSET f, bool skip=false): value(i), flag(f), skip_next_binding(skip) {}
  ideal data() const { return value; }
  BITSET flags() const { return flag; }
  bool skips_binding() const { return skip_next_binding; }
};

class Ideal {
  ideal value;
  ring owner;
  BITSET flag;
  bool skip_next_binding;
public:
  Ideal(ideal i, ring r, BITSET f=0, bool skip=false): value(i), owner(r), flag(f), skip_next_binding(skip) {}
  ~Ideal() { if (value!=nullptr) id_Delete(&value,owner); }
  Ideal(const Ideal &) = delete;
  Ideal &operator=(const Ideal &) = delete;
  Ideal(Ideal &&i) noexcept: value(i.release()), owner(i.owner), flag(i.flag), skip_next_binding(i.skip_next_binding) {}
  Ideal &operator=(Ideal &&i) noexcept {
    if (this!=&i) {
      if (value!=nullptr) id_Delete(&value,owner);
      owner=i.owner; flag=i.flag; skip_next_binding=i.skip_next_binding; value=i.release();
    }
    return *this;
  }
  static Ideal copy(IdealView i, ring r) { return Ideal(id_Copy(i.data(),r),r,i.flags(),i.skips_binding()); }
  IdealView get() const { return IdealView(value,flag,skip_next_binding); }
  ideal data() const { return value; }
  BITSET flags() const { return flag; }
  void flags(BITSET f) { flag=f; }
  bool skips_binding() const { return skip_next_binding; }
  void consume_binding_marker() { skip_next_binding=false; }
  void clear_flags() { flag=0; skip_next_binding=false; }
  ring base() const { return owner; }
  ideal release() { ideal i=value; value=nullptr; return i; }
};

// intvec's implicit copy operations alias its raw allocation. Keep every
// compiler-owned vector behind this move-only owner instead.
class Intvec {
  intvec *value;
public:
  explicit Intvec(intvec *v): value(v) {}
  explicit Intvec(int length=1): value(new intvec(length)) {}
  ~Intvec() { delete value; }
  Intvec(const Intvec &) = delete;
  Intvec &operator=(const Intvec &) = delete;
  Intvec(Intvec &&v) noexcept: value(v.release()) {}
  Intvec &operator=(Intvec &&v) noexcept {
    if (this!=&v) { delete value; value=v.release(); }
    return *this;
  }
  static Intvec copy(const intvec *v) { return Intvec(new intvec(v)); }
  intvec *get() const { return value; }
  intvec *release() { intvec *v=value; value=nullptr; return v; }
};
inline Number number_from_int(long n, ring r) {
  // iiI2N narrows Singular's machine int to C int.
  return Number(n_Init((int)n,r->cf),r->cf);
}
inline Number number_from_poly(poly p, ring r) {
  // Explicit number(poly), jjP2N: a nonconstant polynomial converts to zero.
  return Number(p!=nullptr && p_IsConstant(p,r) ? n_Copy(p_GetCoeff(p,r),r->cf)
                                               : n_Init(0,r->cf),r->cf);
}
inline Number number_neg(number a, ring r) {
  return Number(n_InpNeg(n_Copy(a,r->cf),r->cf),r->cf);
}
inline Number number_neg(Number a, ring) {
  const coeffs cf=a.base();
  return Number(n_InpNeg(a.release(),cf),cf);
}
inline Number number_add(number a, number b, ring r) { return Number(n_Add(a,b,r->cf),r->cf); }
inline Number number_add(Number a, number b, ring) {
  const coeffs cf=a.base();
  number result=a.release();
  n_InpAdd(result,b,cf);
  return Number(result,cf);
}
inline Number number_add(number a, Number b, ring r) {
  return number_add(std::move(b),a,r);
}
inline Number number_sub(number a, number b, ring r) { return Number(n_Sub(a,b,r->cf),r->cf); }
inline Number number_mul(number a, number b, ring r) {
  number n=n_Mult(a,b,r->cf); n_Normalize(n,r->cf); return Number(n,r->cf);
}
inline Number number_mul(Number a, number b, ring) {
  const coeffs cf=a.base();
  number result=a.release();
  n_InpMult(result,b,cf);
  n_Normalize(result,cf);
  return Number(result,cf);
}
inline Number number_mul(number a, Number b, ring r) {
  return number_mul(std::move(b),a,r);
}
inline Number number_div(number a, number b, ring r) {
  if (n_IsZero(b,r->cf)) throw std::runtime_error("div. by 0");
  number n=n_Div(a,b,r->cf); n_Normalize(n,r->cf); return Number(n,r->cf);
}
inline Number number_power(number a, long exponent, ring r) {
  int e=(int)exponent;
  if (e==INT_MIN) throw std::runtime_error("native number exponent out of range");
  Number inverse(e<0 ? n_Invers(a,r->cf) : nullptr,r->cf);
  if (errorreported) throw std::runtime_error("native number inversion failed");
  number result=nullptr;
  n_Power(e<0 ? inverse.get() : a,e<0 ? -e : e,&result,r->cf);
  return Number(result,r->cf);
}
inline Number number_diff(number a, number variable, ring r) {
  if (rIsLPRing(r)) throw std::runtime_error("diff is unsupported in letterplace rings");
  if (!nCoeff_is_transExt(r->cf))
    throw std::runtime_error("differentiation not defined in the coefficient ring");
  return Number(ntDiff(a,variable,r->cf),r->cf);
}
inline Number fraction_part(number a, bool denominator, ring r) {
  number copy=n_Copy(a,r->cf);
  number part=denominator ? n_GetDenom(copy,r->cf) : n_GetNumerator(copy,r->cf);
  n_Delete(&copy,r->cf);
  return Number(part,r->cf);
}
inline Poly polynomial(number n, ring r) { return Poly(p_NSet(n_Copy(n,r->cf),r),r); }
inline Poly divide(poly a, poly b, ring r) {
  if (b==nullptr) throw std::runtime_error("div. by 0");
  poly p=pp_Divide(a,b,r);
  if (p!=nullptr) p_Normalize(p,r);
  return Poly(p,r);
}
inline Poly substitute(poly p, poly variable, poly image, ring r) {
  int v=p_Var(variable,r);
  if (v==0 && variable!=nullptr && r->cf->extRing!=nullptr)
    v=-n_IsParam(p_GetCoeff(variable,r),r);
  if (v==0) throw std::runtime_error("ringvar/par expected");
  if (v<0) {
    if (rIsLPRing(r)) throw std::runtime_error("Substituting parameters not implemented for Letterplace rings.");
    return Poly(pSubstPar(p,-v,image),r);
  }
  const int degree=p_MaxExpPerVar(p,v,r);
  if (!rIsLPRing(r) && image!=nullptr && p!=nullptr && degree!=0 &&
      (unsigned long)p_Totaldegree(image,r)>r->bitmask/(unsigned long)degree/2)
    WarnS("possible OVERFLOW in native subst");
  return Poly(image==nullptr || pNext(image)==nullptr ? p_Subst(p_Copy(p,r),v,image,r)
                                                      : pSubstPoly(p,v,image),r);
}
inline Poly add(Poly a, Poly b) {
  const ring r=a.base();
  return Poly(p_Add_q(a.release(),b.release(),r),r);
}
inline Poly neg(Poly a) {
  const ring r=a.base();
  return Poly(p_Neg(a.release(),r),r);
}
inline Poly sub(Poly a, Poly b) { return add(std::move(a),neg(std::move(b))); }
inline Poly mul(Poly a, Poly b) {
  const ring r=a.base();
  if (!rIsLPRing(r) && a.get()!=nullptr && b.get()!=nullptr &&
      p_Totaldegree(a.get(),r)>si_max((long)rVar(r),(long)r->bitmask/2)-p_Totaldegree(b.get(),r))
    Warn("possible OVERFLOW in mult(d=%ld, d=%ld, max=%ld)",
         p_Totaldegree(a.get(),r),p_Totaldegree(b.get(),r),r->bitmask/2);
  return Poly(p_Mult_q(a.release(),b.release(),r),r);
}
inline Poly power(Poly a, long exponent) {
  const ring r=a.base();
  const int e=(int)exponent; // same narrowing as jjPOWER_P
  if (e<0) throw std::runtime_error("exponent must be non-negative");
  if (a.get()!=nullptr && !rIsLPRing(r) && e!=0 &&
      p_Totaldegree(a.get(),r)>(signed long)r->bitmask/e/2)
    throw std::runtime_error("OVERFLOW in native polynomial power");
  return Poly(p_Power(a.release(),e,r),r);
}
inline Poly variable(long index, ring r) {
  const int i=(int)index;
  if (i<=0 || i>r->N) throw std::runtime_error("native var index out of range");
  Poly p(p_One(r),r);
  p_SetExp(p.get(),i,1,r);
  p_Setm(p.get(),r);
  return p;
}
inline Poly lead(poly p, ring r) { return Poly(p_Head(p,r),r); }
inline Number leadcoef(poly p, ring r) {
  number result=p==nullptr ? n_Init(0,r->cf) : n_Copy(p_GetCoeff(p,r),r->cf);
  n_Normalize(result,r->cf);
  return Number(result,r->cf);
}
inline Intvec leadexp(poly p, ring r) {
  Intvec result(r->N);
  if (p!=nullptr)
    for (int i=r->N;i>0;--i) (*result.get())[i-1]=p_GetExp(p,i,r);
  return result;
}
inline Poly drop_lead(Poly p) {
  const ring r=p.base();
  poly value=p.release();
  return Poly(value==nullptr ? nullptr : p_LmDeleteAndNext(value,r),r);
}
inline Poly leadmonom(poly p, ring r) {
  Poly result(p_Head(p,r),r);
  if (result.get()!=nullptr) p_SetCoeff(result.get(),n_Init(1,r->cf),r);
  return result;
}
inline Poly term(poly p, long index, ring r) {
  // jjINDEX_P returns zero for all out-of-range indices, including <=0.
  if (index<=0) return Poly(nullptr,r);
  for (long i=1; p!=nullptr && i<index; ++i) p=pNext(p);
  return lead(p,r);
}
inline Poly jet(Poly p, long degree) {
  const ring r=p.base();
  if (rIsLPRing(r)) throw std::runtime_error("jet is unsupported in letterplace rings");
  return Poly(p_Jet(p.release(),(int)degree,r),r);
}

inline long add(long a, long b) {
  const unsigned long x=a, y=b, z=x+y;
  const unsigned long sign=1UL << (sizeof(long)*CHAR_BIT-1);
  if (((x&sign)==(y&sign)) && ((x&sign)!=(z&sign)))
    WarnS("int overflow(+), result may be wrong");
  return (long)z;
}
inline long sub(long a, long b) {
  const unsigned long x=a, y=b, z=x-y;
  const unsigned long sign=1UL << (sizeof(long)*CHAR_BIT-1);
  if (((x&sign)!=(y&sign)) && ((x&sign)!=(z&sign)))
    WarnS("int overflow(-), result may be wrong");
  return (long)z;
}
inline long mul(long a, long b) {
  const long z=(long)((unsigned long)a*(unsigned long)b);
  if ((a==-1 && z==LONG_MIN) || (a!=0 && z/a!=b))
    WarnS("int overflow(*), result may be wrong");
  return z;
}
inline long integer_size(long value) {
  // COUNT_CMD reaches an int through iiI2BI, which narrows to C int before
  // constructing the bigint.  A nonzero machine int has one bigint limb.
  return (int)value==0 ? 0 : 1;
}
inline long integer_power(long base, long exponent) {
  if (exponent<0) throw std::runtime_error("exponent must be non-negative");
  if (base==0) return exponent==0;
  if (exponent==0 || base==1) return 1;
  if (base==-1) return (exponent&1) ? -1 : 1;
  long result=1;
  bool overflow=false;
  while (exponent--!=0) {
    const long previous=result;
    result=(long)((unsigned long)result*(unsigned long)base);
    if (!overflow && result/base!=previous) overflow=true;
  }
  if (overflow) WarnS("int overflow(^), result may be wrong");
  return result;
}
inline long neg(long a) { return (long)(0UL-(unsigned long)a); }
inline long increment(long a, int step) {
  return (int)((unsigned long)a+(unsigned long)step);
}
inline bool truth(long a) { return (int)a!=0; }
inline void declare_local(int &declared_type, int type, bool declared_as_def,
                          const char *name) {
  if (declared_type!=0) {
    if (!declared_as_def && declared_type!=type)
      throw std::runtime_error(std::string("identifier `")+name+"` in use");
    if (BVERBOSE(V_REDEFINE)) Warn("redefining %s",name);
  }
  // A def declaration adopts its initializer's concrete type.  It may change
  // an existing identifier's type, but a later explicit declaration must then
  // agree with that new concrete type.
  declared_type=type;
}

inline String string_add(const char *a, const char *b) {
  if (a==nullptr) a="";
  if (b==nullptr) b="";
  const size_t left=strlen(a), right=strlen(b);
  if (right>SIZE_MAX-left-1) throw std::runtime_error("native string is too large");
  char *result=(char*)omAlloc(left+right+1);
  memcpy(result,a,left);
  memcpy(result+left,b,right+1);
  return String(result);
}
inline long string_compare(const char *a, const char *b, int operation) {
  const int comparison=strcmp(a==nullptr ? "" : a,b==nullptr ? "" : b);
  switch (operation) {
    case '<': return comparison<0;
    case '>': return comparison>0;
    case 1: return comparison<=0;
    case 2: return comparison>=0;
    case 3: return comparison==0;
    case 4: return comparison!=0;
  }
  throw std::runtime_error("unsupported native string comparison");
}
inline String string_index(const char *value, int index) {
  if (value==nullptr) value="";
  char *result=(char*)omAlloc(2);
  result[0]=(index>0 && index<=(long)strlen(value)) ? value[index-1] : '\0';
  result[1]='\0';
  return String(result);
}
inline void string_set(String &target, int index, const char *value) {
  char *text=target.write();
  const long length=text==nullptr ? 0L : (long)strlen(text);
  if (index<=0 || index>length)
    throw std::runtime_error("native string index out of range");
  text[index-1]=(value==nullptr ? '\0' : value[0]);
}
inline long string_to_int(const char *value) { return (long)atoi(value==nullptr ? "" : value); }

inline Ideal ideal_from_poly(poly value, ring r) {
  ideal result=idInit(1,1);
  result->m[0]=p_Copy(value,r);
  return Ideal(result,r);
}
inline Ideal ideal_from_int(long value, ring r) {
  ideal result=idInit(1,1);
  result->m[0]=p_ISet((int)value,r);
  return Ideal(result,r);
}
inline Ideal ideal_from_number(number value, ring r) {
  ideal result=idInit(1,1);
  result->m[0]=p_NSet(n_Copy(value,r->cf),r);
  id_Normalize(result,r);
  // NUMBER_CMD reaches an ideal through NUMBER->MATRIX->IDEAL.  This first
  // boundary does not add the ordinary one-column STD flag; a subsequent
  // exact IDEAL boundary does.  Unlike a typed polynomial formal, neither
  // ideal boundary reduces data modulo a quotient ideal.
  return Ideal(result,r,0,true);
}
inline long ideal_size(IdealView value) {
  if (value.data()==nullptr) throw std::runtime_error("native ideal value is invalid");
  return idElem(value.data());
}
inline long ideal_columns(IdealView value) {
  if (value.data()==nullptr) throw std::runtime_error("native ideal value is invalid");
  return MATCOLS((matrix)value.data());
}
inline BITSET ideal_assignment_flags(IdealView value, ring r) {
  BITSET flags=value.flags();
  if (value.data()!=nullptr && IDELEMS(value.data())==1 && r->qideal==nullptr &&
      !rIsPluralRing(r))
    flags|=Sy_bit(FLAG_STD);
  return flags;
}
inline Ideal ideal_assign(Ideal value, ring r) {
  if (value.skips_binding()) {
    value.consume_binding_marker();
    return std::move(value);
  }
  ideal data=value.release();
  BITSET flags=value.flags();
  if (data==nullptr) throw std::runtime_error("native ideal value is invalid");
  id_Normalize(data,r);
  IdealView normalized(data,flags);
  flags=ideal_assignment_flags(normalized,r);
  return Ideal(data,r,flags);
}
class IdealBinding {
  IdealView value;
public:
  IdealBinding(IdealView input, ring r): value(input) {
    if (input.data()==nullptr)
      throw std::runtime_error("native ideal value is invalid");
    if (input.skips_binding()) {
      value=IdealView(input.data(),input.flags());
    } else {
      value=IdealView(input.data(),ideal_assignment_flags(input,r));
    }
  }
  IdealBinding(const IdealBinding &)=delete;
  IdealBinding &operator=(const IdealBinding &)=delete;
  IdealView get() const { return value; }
};
inline void append(Ideal &target, poly value) {
  ideal result=target.data();
  const int old=IDELEMS(result);
  if (old==INT_MAX) throw std::runtime_error("native ideal capacity out of range");
  pEnlargeSet(&result->m,old,1);
  IDELEMS(result)=old+1;
  result->m[old]=p_Copy(value,target.base());
}
inline void append(Ideal &target, Poly value) {
  ideal result=target.data();
  const int old=IDELEMS(result);
  if (old==INT_MAX) throw std::runtime_error("native ideal capacity out of range");
  pEnlargeSet(&result->m,old,1);
  IDELEMS(result)=old+1;
  result->m[old]=value.release();
}
inline void append(Ideal &target, IdealView value) {
  if (value.data()==nullptr) throw std::runtime_error("native ideal value is invalid");
  const int old=IDELEMS(target.data()), extra=IDELEMS(value.data());
  if (extra>INT_MAX-old) throw std::runtime_error("native ideal capacity out of range");
  if (extra==0) return;
  pEnlargeSet(&target.data()->m,old,extra);
  IDELEMS(target.data())=old+extra;
  for (int i=0;i<extra;++i)
    target.data()->m[old+i]=p_Copy(value.data()->m[i],target.base());
}
inline Poly ideal_get(IdealView value, int index, ring r) {
  if (value.data()==nullptr || index<=0 || index>IDELEMS(value.data()))
    throw std::runtime_error("native ideal index out of range");
  return Poly(p_Copy(value.data()->m[index-1],r),r);
}
inline void ideal_set(Ideal &target, int index, Poly value) {
  // iiAssign clears IDFLAG before dispatching every indexed assignment.
  target.clear_flags();
  if (index<=0)
    throw std::runtime_error("native ideal index out of range");
  ideal result=target.data();
  const int old=IDELEMS(result);
  if (index>old) {
    pEnlargeSet(&result->m,old,index-old);
    IDELEMS(result)=index;
  }
  poly replacement=value.release();
  if (replacement!=nullptr) p_Normalize(replacement,target.base());
  if (replacement!=nullptr && TEST_V_QRING && target.base()->qideal!=nullptr)
    replacement=jj_NormalizeQRingP(replacement,target.base());
  p_Delete(&result->m[index-1],target.base());
  result->m[index-1]=replacement;
}
inline Ideal ideal_add(IdealView a, IdealView b, ring r) {
  return Ideal(id_Add(a.data(),b.data(),r),r);
}
inline Ideal ideal_mul(IdealView a, IdealView b, ring r) {
  return Ideal(id_Mult(a.data(),b.data(),r),r);
}
inline Ideal ideal_mul_poly(IdealView a, poly b, ring r) {
  return Ideal((ideal)mp_MultP((matrix)id_Copy(a.data(),r),p_Copy(b,r),r),r);
}
inline Ideal poly_mul_ideal(poly a, IdealView b, ring r) {
  return Ideal((ideal)pMultMp(p_Copy(a,r),(matrix)id_Copy(b.data(),r),r),r);
}
inline Ideal ideal_power(IdealView value, long exponent, ring r) {
  return Ideal(id_Power(value.data(),(int)exponent,r),r);
}
inline poly borrow_poly(leftv argument) {
  void *data=argument->Data();
  if (errorreported) throw std::runtime_error("native polynomial argument is invalid");
  return argument->Typ()==BUCKET_CMD ? sBucketPeek((sBucket_pt)data) : (poly)data;
}
inline bool metadata(leftv argument) {
  if (argument->attribute!=NULL || argument->flag!=0) return true;
  if (argument->rtyp==IDHDL) {
    idhdl id=(idhdl)argument->data;
    return id==NULL || IDATTR(id)!=NULL || IDFLAG(id)!=0;
  }
  return false;
}
inline bool attributed(leftv argument) {
  if (argument->attribute!=NULL) return true;
  return argument->rtyp==IDHDL &&
         (((idhdl)argument->data)==NULL || IDATTR((idhdl)argument->data)!=NULL);
}
inline BITSET value_flags(leftv argument) {
  if (argument==nullptr) return 0;
  if (argument->rtyp==IDHDL) {
    idhdl id=(idhdl)argument->data;
    return id==nullptr ? 0 : IDFLAG(id);
  }
  return argument->flag;
}
inline bool unsupported_poly_result_metadata(leftv argument) {
  // FLAG_RING is an internal marker carried by values read from a
  // ring-dependent newstruct field.  It does not describe the polynomial and
  // must not make an otherwise ordinary typed procedure call fail.  Keep all
  // user attributes and every semantic flag (including FLAG_QRING) on the
  // conservative path; FLAG_OTHER_RING is deliberately not ignored either.
  if (argument==nullptr) return true;
  const BITSET semantic=~Sy_bit(FLAG_RING);
  return attributed(argument) ||
         (argument->flag & semantic)!=0 ||
         (value_flags(argument) & semantic)!=0;
}
inline leftv get(lists list, int index) {
  if (index<=0 || index>list->nr+1)
    throw std::runtime_error("native list index out of range");
  return &list->m[index-1];
}
inline lists list_like_data(leftv value) {
  if (value==nullptr) return nullptr;
  int type=value->rtyp;
  void *data=value->data;
  if (type==IDHDL) {
    idhdl id=(idhdl)data;
    if (id==nullptr) return nullptr;
    type=IDTYP(id);
    data=IDDATA(id);
  }
  if (type==LIST_CMD) return (lists)data;
  if (type>MAX_TOK) {
    blackbox *box=getBlackboxStuff(type);
    if (box!=nullptr && BB_LIKE_LIST(box)) return (lists)data;
  }
  return nullptr;
}
inline leftv list_element_argument(leftv argument) {
  if (argument==nullptr || argument->e==nullptr) return argument;
  // Resolve nested list and list-like blackbox selections without changing the
  // original leftv or its elements. Newstruct values use Singular's ordinary
  // BB_LIKE_LIST representation, so this also materializes a field passed to a
  // native entry (for example v.rho where rho is an intvec). Unlike general
  // Data()/LData() indexing this cannot manufacture a temporary coefficient or
  // term, nor can it invoke an arbitrary blackbox indexing callback.
  sleftv head=*argument;
  head.e=nullptr;
  leftv current=&head;
  for (Subexpr index=argument->e; index!=nullptr; index=index->next) {
    lists list=list_like_data(current);
    if (list==nullptr)
      throw std::runtime_error("native indexed argument requires a list or list-like field selection");
    current=get(list,index->start);
  }
  return current;
}
inline lists list_argument(leftv argument) {
  argument=list_element_argument(argument);
  if (argument==nullptr || (argument->rtyp!=LIST_CMD && argument->rtyp!=IDHDL) ||
      argument->e!=nullptr || argument->Typ()!=LIST_CMD)
    throw std::runtime_error("native argument requires list");
  lists list=(lists)argument->Data();
  if (errorreported || list==nullptr) throw std::runtime_error("native list argument is invalid");
  return list;
}
inline lists plain_list_argument(leftv argument) {
  argument=list_element_argument(argument);
  if (argument!=nullptr && metadata(argument))
    throw std::runtime_error("attributed list values for list-producing operations are unsupported");
  return list_argument(argument);
}

// Owned list storage and borrowed typed views. Native-to-native calls borrow;
// only Singular value-producing operations perform deep copies.
class List {
  lists value;
  ring owner;
  int allocated;

  void destroy() {
    if (value==nullptr) return;
    for (int i=value->nr;i>=0;--i)
      if (value->m[i].rtyp!=DEF_CMD) value->m[i].CleanUp(owner);
    if (value->m!=nullptr)
      omFreeSize((ADDRESS)value->m,(size_t)allocated*sizeof(sleftv));
    omFreeBin((ADDRESS)value,slists_bin);
    value=nullptr;
    allocated=0;
  }
  void compact() {
    if (value==nullptr) return;
    const int logical=value->nr+1;
    if (allocated==logical) return;
    if (logical==0) {
      if (value->m!=nullptr)
        omFreeSize((ADDRESS)value->m,(size_t)allocated*sizeof(sleftv));
      value->m=nullptr;
    } else {
      value->m=(sleftv*)omReallocSize(value->m,
                                      (size_t)allocated*sizeof(sleftv),
                                      (size_t)logical*sizeof(sleftv));
    }
    allocated=logical;
  }
public:
  explicit List(ring r): value((lists)omAlloc0Bin(slists_bin)), owner(r), allocated(0) {
    value->Init();
  }
  List(lists l, ring r): value(l), owner(r), allocated(l==nullptr ? 0 : l->nr+1) {}
  ~List() { destroy(); }
  List(const List &) = delete;
  List &operator=(const List &) = delete;
  List(List &&l) noexcept: value(l.value), owner(l.owner), allocated(l.allocated) {
    l.value=nullptr;
    l.allocated=0;
  }
  List &operator=(List &&l) noexcept {
    if (this!=&l) {
      destroy();
      value=l.value;
      owner=l.owner;
      allocated=l.allocated;
      l.value=nullptr;
      l.allocated=0;
    }
    return *this;
  }
  static List copy(lists l, ring r) { return List(lCopy(l),r); }
  lists get() const { return value; }
  void reserve(long required) {
    if (required<0 || required>INT_MAX)
      throw std::runtime_error("native list capacity out of range");
    if (required<=allocated) return;
    long grown=allocated==0 ? 4L :
      (allocated>INT_MAX/2 ? (long)INT_MAX : (long)allocated*2L);
    if (grown<required) grown=required;
    if (grown>INT_MAX) grown=INT_MAX;
    value->m=(sleftv*)(allocated==0
      ? omAlloc0((size_t)grown*sizeof(sleftv))
      : omRealloc0Size(value->m,(size_t)allocated*sizeof(sleftv),
                       (size_t)grown*sizeof(sleftv)));
    // omRealloc0Size may keep allocator padding between the old requested
    // size and a rounded block boundary.  That padding becomes live sleftv
    // storage after a capacity increase, so initialize it explicitly rather
    // than relying on the allocator to clear every newly exposed slot.
    if (grown>allocated)
      std::memset(value->m+allocated,0,
                  (size_t)(grown-allocated)*sizeof(sleftv));
    allocated=(int)grown;
  }
  lists release() {
    // Raw slists do not carry a capacity. Restore the exact allocation size
    // expected by slists::Clean before the value crosses the native boundary.
    compact();
    lists l=value;
    value=nullptr;
    allocated=0;
    return l;
  }
};

// Procedure parameters have Singular value semantics. Reads remain zero-copy;
// the first indexed write takes a private copy, while direct assignment can
// install an already-evaluated owner without copying the old value.
class CowPoly {
  poly borrowed;
  Poly owned;
  bool is_owned;
public:
  CowPoly(poly p, ring r): borrowed(p), owned(nullptr,r), is_owned(false) {}
  poly get() const { return is_owned ? owned.get() : borrowed; }
  void assign(Poly value) { owned=std::move(value); is_owned=true; }
};
class CowNumber {
  number borrowed;
  Number owned;
  bool is_owned;
public:
  CowNumber(number n, ring r): borrowed(n), owned(nullptr,r->cf), is_owned(false) {}
  number get() const { return is_owned ? owned.get() : borrowed; }
  void assign(Number value) { owned=std::move(value); is_owned=true; }
};
class CowString {
  const char *borrowed;
  String owned;
  bool is_owned;
public:
  explicit CowString(const char *s): borrowed(s), owned((char*)nullptr), is_owned(false) {}
  const char *get() const { return is_owned ? owned.get() : (borrowed==nullptr ? "" : borrowed); }
  String &write() {
    if (!is_owned) { owned=String::copy(borrowed); is_owned=true; }
    return owned;
  }
  void assign(String value) { owned=std::move(value); is_owned=true; }
};
class CowIdeal {
  IdealView borrowed;
  Ideal owned;
  ring base;
  bool is_owned;
public:
  CowIdeal(IdealView i, ring r): borrowed(i), owned((ideal)nullptr,r), base(r), is_owned(false) {}
  IdealView get() const { return is_owned ? owned.get() : borrowed; }
  Ideal &write() {
    if (!is_owned) { owned=Ideal::copy(borrowed,base); is_owned=true; }
    return owned;
  }
  void assign(Ideal value) { owned=std::move(value); is_owned=true; }
};
class CowList {
  lists borrowed;
  List owned;
  ring base;
  bool is_owned;
public:
  CowList(lists l, ring r): borrowed(l), owned((lists)nullptr,r), base(r), is_owned(false) {}
  lists get() const { return is_owned ? owned.get() : borrowed; }
  List &write() {
    if (!is_owned) { owned=List::copy(borrowed,base); is_owned=true; }
    return owned;
  }
  void assign(List value) { owned=std::move(value); is_owned=true; }
};
class CowIntvec {
  intvec *borrowed;
  Intvec owned;
  bool is_owned;
public:
  explicit CowIntvec(intvec *v): borrowed(v), owned((intvec*)nullptr), is_owned(false) {}
  intvec *get() const { return is_owned ? owned.get() : borrowed; }
  Intvec &write() {
    if (!is_owned) { owned=Intvec::copy(borrowed); is_owned=true; }
    return owned;
  }
  void assign(Intvec value) { owned=std::move(value); is_owned=true; }
};
struct View {
  sleftv value;
  View(int type, void *data) { value.Init(); value.rtyp=type; value.data=data; }
  explicit View(IdealView ideal) {
    value.Init(); value.rtyp=IDEAL_CMD; value.data=ideal.data(); value.flag=ideal.flags();
  }
  leftv get() { return &value; }
};
class Value {
  sleftv value;
  ring owner;
public:
  explicit Value(ring r): owner(r) { value.Init(); }
  ~Value() { value.CleanUp(owner); }
  Value(const Value &) = delete;
  Value &operator=(const Value &) = delete;
  Value(Value &&v) noexcept: value(v.value), owner(v.owner) { v.value.Init(); }
  leftv get() { return &value; }
};
// Marks an error already diagnosed by an interpreter kernel.  Native entry
// wrappers return it directly instead of printing a second, less useful
// exception message on top of the original Singular diagnostic.
struct ReportedInterpreterError {};
inline leftv value_argument(leftv argument) {
  argument=list_element_argument(argument);
  if (argument==nullptr || argument->e!=nullptr || argument->rtyp==ALIAS_CMD)
    throw std::runtime_error("native argument requires a plain value or list selection");
  return argument;
}
inline leftv newstruct_argument(leftv argument, const char *expected) {
  argument=value_argument(argument);
  const int type=argument->Typ();
  blackbox *box=getBlackboxStuff(type);
  const char *actual=type>MAX_TOK ? getBlackboxName(type) : nullptr;
  if (box==nullptr || box->blackbox_Op2!=newstruct_Op2 || box->data==nullptr ||
      actual==nullptr || std::strcmp(actual,expected)!=0)
    throw std::runtime_error(std::string("native argument requires newstruct ")+
                             expected);
  return argument;
}
inline String string_from_value(leftv argument) {
  argument=value_argument(argument);
  char *raw=argument->String();
  String result(raw);
  if (errorreported || raw==nullptr)
    throw std::runtime_error("native string conversion failed");
  return result;
}
inline leftv scalar_index_argument(leftv argument, sleftv &materialized,
                                   char string_value[2]) {
  // A scalar selection is represented by a value plus a Subexpr chain. Walk
  // only a list prefix, then materialize one supported final scalar index.
  // This covers v[i], I[i], and carrier[...][i] without invoking general
  // Data()/Copy on an indexed leftv (which may rewrite or free interpreter
  // owned state).
  if (argument==nullptr || argument->e==nullptr) return argument;
  sleftv head=*argument;
  head.e=nullptr;
  leftv current=&head;
  Subexpr index=argument->e;
  while (true) {
    if (current->e!=nullptr) return argument;
    int type=current->rtyp;
    void *data=current->data;
    if (type==IDHDL && data!=nullptr) {
      type=IDTYP((idhdl)data);
      data=IDDATA((idhdl)data);
    }
    // An interpreted caller can pass M[row,col] directly to any compiled
    // scalar parameter.  Singular represents that selection as two Subexpr
    // nodes on the intmat rather than materializing an int first.  Handle only
    // that exact scalar boundary form; native intmat ownership and expression
    // lowering remain intentionally unsupported.
    if (type==INTMAT_CMD && index->next!=nullptr &&
        index->next->next==nullptr) {
      intvec *source=(intvec*)data;
      const int row=index->start;
      const int column=index->next->start;
      if (source==nullptr || row<1 || row>source->rows() ||
          column<1 || column>source->cols())
        throw std::runtime_error("native intmat index out of range");
      materialized.Init();
      materialized.rtyp=INT_CMD;
      materialized.data=(void*)(long)IMATELEM((*source),row,column);
      return &materialized;
    }
    if (index->next==nullptr) break;
    lists prefix=list_like_data(current);
    if (prefix==nullptr) return argument;
    current=get(prefix,index->start);
    index=index->next;
  }
  if (current->e!=nullptr) return argument;
  int type=current->rtyp;
  void *data=current->data;
  if (type==IDHDL && data!=nullptr) {
    type=IDTYP((idhdl)data);
    data=IDDATA((idhdl)data);
  }
  const int position=index->start;
  materialized.Init();
  if (type==INTVEC_CMD) {
    intvec *source=(intvec*)data;
    if (source==nullptr || position<1 || position>source->length())
      throw std::runtime_error("native intvec index out of range");
    materialized.rtyp=INT_CMD;
    materialized.data=(void*)(long)(*source)[position-1];
    return &materialized;
  }
  // Singular keeps a scalar string selection deferred in the argument leftv.
  // Give callers a non-owning view backed by their stack storage; callers
  // either copy it immediately (for list values) or keep that storage alive
  // for the duration of the native call.  This avoids Data(), whose string
  // indexing path rewrites and cleans the interpreter-owned leftv.
  if (type==STRING_CMD) {
    const char *source=(const char*)data;
    string_value[0]=(source!=nullptr && position>0 &&
                     position<=(int)strlen(source)) ? source[position-1] : '\0';
    string_value[1]='\0';
    materialized.rtyp=STRING_CMD;
    materialized.data=(void*)string_value;
    return &materialized;
  }
  // Ideal/module scalar indexing is another deferred LANG_C boundary value.
  // Borrow the selected polynomial only long enough for the caller to copy or
  // consume it; unlike Data(), this neither rewrites the leftv nor allocates.
  if (type==IDEAL_CMD || type==MODUL_CMD || type==MAP_CMD) {
    ideal source=(ideal)data;
    if (source==nullptr || position<1 || position>IDELEMS(source))
      throw std::runtime_error("native ideal/module index out of range");
    materialized.rtyp=type==MODUL_CMD ? VECTOR_CMD : POLY_CMD;
    materialized.data=(void*)source->m[position-1];
    return &materialized;
  }
  return argument;
}
class PolyArgument {
  Poly converted;
  poly value;
public:
  PolyArgument(leftv argument, ring r): converted(nullptr,r), value(nullptr) {
    sleftv materialized;
    char string_value[2];
    argument=scalar_index_argument(argument,materialized,string_value);
    argument=value_argument(argument);
    const int type=argument->Typ();
    if (type==POLY_CMD || type==BUCKET_CMD) {
      value=borrow_poly(argument);
    } else if (type==INT_CMD) {
      void *data=argument->Data();
      if (errorreported) throw ReportedInterpreterError();
      converted=Poly(p_ISet((int)(long)data,r),r);
      if (errorreported) throw ReportedInterpreterError();
      value=converted.get();
    } else if (type==NUMBER_CMD) {
      number data=(number)argument->Data();
      if (errorreported) throw ReportedInterpreterError();
      Number copied(n_Copy(data,r->cf),r->cf);
      if (errorreported) throw ReportedInterpreterError();
      converted=Poly(p_NSet(copied.release(),r),r);
      if (errorreported) throw ReportedInterpreterError();
      value=converted.get();
    } else if (type==BIGINT_CMD) {
      number data=(number)argument->Data();
      nMapFunc map=n_SetMap(coeffs_BIGINT,r->cf);
      if (errorreported) throw ReportedInterpreterError();
      if (map==nullptr)
        throw std::runtime_error("native bigint-to-poly argument conversion failed");
      Number mapped(map(data,coeffs_BIGINT,r->cf),r->cf);
      if (errorreported) throw ReportedInterpreterError();
      converted=Poly(p_NSet(mapped.release(),r),r);
      if (errorreported) throw ReportedInterpreterError();
      value=converted.get();
    } else {
      throw std::runtime_error("native polynomial argument requires poly, number, bigint, or int");
    }
  }
  PolyArgument(const PolyArgument &)=delete;
  PolyArgument &operator=(const PolyArgument &)=delete;
  poly get() const { return value; }
};
inline long int_argument(leftv argument) {
  sleftv materialized;
  char string_value[2];
  argument=scalar_index_argument(argument,materialized,string_value);
  argument=value_argument(argument);
  if (argument->Typ()!=INT_CMD || metadata(argument))
    throw std::runtime_error("native value requires a plain int");
  void *data=argument->Data();
  if (errorreported) throw std::runtime_error("native int argument is invalid");
  return (long)data;
}
inline long power_exponent(leftv argument) {
  argument=value_argument(argument);
  if (argument->Typ()!=INT_CMD)
    throw std::runtime_error("native exponent requires int");
  void *data=argument->Data();
  if (errorreported) throw ReportedInterpreterError();
  // jjPOWER_P/N/ID consume only Data(); attributes and flags on the integer
  // operand do not participate in the operation.
  return (long)data;
}
inline long compare_ints(long left, long right, int operation) {
  // Singular's INT_CMD kernels compare C ints even though the interpreter
  // stores them in pointer-sized slots.
  const int a=(int)left, b=(int)right;
  switch (operation) {
    case '<': return a<b;
    case '>': return a>b;
    case LE: return a<=b;
    case GE: return a>=b;
    case EQUAL_EQUAL: return a==b;
    case NOTEQUAL: return a!=b;
  }
  throw std::runtime_error("unsupported native integer comparison");
}
inline long compare_value_int(leftv argument, long integer, int operation,
                              bool value_on_left, ring r) {
  sleftv materialized;
  char string_value[2];
  argument=scalar_index_argument(argument,materialized,string_value);
  argument=value_argument(argument);

  // This is the hot path for a def/list value guarded by typeof(...), as in
  // ellipticcovers.lib::propagator.  Keep it entirely in generated C++.
  const int argument_type=argument->Typ();
  if (errorreported) throw ReportedInterpreterError();
  if (argument_type==INT_CMD) {
    void *data=argument->Data();
    if (errorreported) throw ReportedInterpreterError();
    const long dynamic=(long)data;
    return value_on_left ? compare_ints(dynamic,integer,operation)
                         : compare_ints(integer,dynamic,operation);
  }

  // A runtime value can also select Singular's number, bigint, polynomial,
  // intvec, string, blackbox, or other comparison overload.  Deep-copy both
  // operands before asking the ordinary binary dispatcher so its mandatory
  // cleanup cannot touch interpreter- or list-owned data.  This cold path is
  // deliberately exact rather than guessing an overload from the result type.
  Value left(r), right(r), result(r);
  leftv dynamic=value_on_left ? left.get() : right.get();
  leftv scalar=value_on_left ? right.get() : left.get();
  // A direct def formal points into the native entry's argument chain.  The
  // following arguments are not part of this expression operand.
  sleftv head=*argument;
  head.next=nullptr;
  dynamic->Copy(&head);
  if (errorreported) throw ReportedInterpreterError();
  scalar->rtyp=INT_CMD;
  scalar->data=(void*)integer;
  struct RestoreOperation {
    int saved;
    RestoreOperation(): saved(iiOp) {}
    ~RestoreOperation() { iiOp=saved; }
  } restore_operation;
  const BOOLEAN failed=iiExprArith2(result.get(),left.get(),operation,right.get());
  if (errorreported) throw ReportedInterpreterError();
  if (failed || result.get()->Typ()!=INT_CMD)
    throw std::runtime_error("native dynamic comparison failed");
  void *data=result.get()->Data();
  if (errorreported) throw ReportedInterpreterError();
  return (long)data;
}
inline long compare_values(leftv first, leftv second, int operation, ring r) {
  sleftv first_materialized, second_materialized;
  char first_string[2], second_string[2];
  first=scalar_index_argument(first,first_materialized,first_string);
  second=scalar_index_argument(second,second_materialized,second_string);
  first=value_argument(first);
  second=value_argument(second);
  const int first_type=first->Typ(), second_type=second->Typ();
  if (errorreported) throw ReportedInterpreterError();
  if (first_type==INT_CMD && second_type==INT_CMD) {
    void *a=first->Data(), *b=second->Data();
    if (errorreported) throw ReportedInterpreterError();
    return compare_ints((long)a,(long)b,operation);
  }

  Value left(r), right(r), result(r);
  sleftv first_head=*first, second_head=*second;
  first_head.next=nullptr;
  second_head.next=nullptr;
  left.get()->Copy(&first_head);
  if (errorreported) throw ReportedInterpreterError();
  right.get()->Copy(&second_head);
  if (errorreported) throw ReportedInterpreterError();
  struct RestoreOperation {
    int saved;
    RestoreOperation(): saved(iiOp) {}
    ~RestoreOperation() { iiOp=saved; }
  } restore_operation;
  const BOOLEAN failed=iiExprArith2(result.get(),left.get(),operation,right.get());
  if (errorreported) throw ReportedInterpreterError();
  if (failed || result.get()->Typ()!=INT_CMD)
    throw std::runtime_error("native dynamic comparison failed");
  void *data=result.get()->Data();
  if (errorreported) throw ReportedInterpreterError();
  return (long)data;
}
inline long size_value(leftv argument, ring r) {
  sleftv materialized;
  char string_value[2];
  argument=scalar_index_argument(argument,materialized,string_value);
  argument=value_argument(argument);
  const int type=argument->Typ();
  if (errorreported) throw ReportedInterpreterError();
  void *data=argument->Data();
  if (errorreported) throw ReportedInterpreterError();
  switch (type) {
    case POLY_CMD:
    case VECTOR_CMD: return pLength((poly)data);
    case IDEAL_CMD:
    case MODUL_CMD: return idElem((ideal)data);
    case INTVEC_CMD:
    case INTMAT_CMD: return ((intvec*)data)->length();
    case LIST_CMD: return lSize((lists)data)+1;
    case STRING_CMD: return strlen((const char*)data);
  }

  Value input(r), result(r);
  sleftv head=*argument;
  head.next=nullptr;
  input.get()->Copy(&head);
  if (errorreported) throw ReportedInterpreterError();
  struct RestoreOperation {
    int saved;
    RestoreOperation(): saved(iiOp) {}
    ~RestoreOperation() { iiOp=saved; }
  } restore_operation;
  const BOOLEAN failed=iiExprArith1(result.get(),input.get(),COUNT_CMD);
  if (errorreported) throw ReportedInterpreterError();
  if (failed || result.get()->Typ()!=INT_CMD)
    throw std::runtime_error("native dynamic size failed");
  data=result.get()->Data();
  if (errorreported) throw ReportedInterpreterError();
  return (long)data;
}
inline Value unary_value(leftv argument, int operation, ring r) {
  sleftv materialized;
  char string_value[2];
  argument=scalar_index_argument(argument,materialized,string_value);
  argument=value_argument(argument);
  const int type=argument->Typ();
  if (errorreported) throw ReportedInterpreterError();

  Value result(r);
  if (type==INT_CMD && operation=='-') {
    void *data=argument->Data();
    if (errorreported) throw ReportedInterpreterError();
    result.get()->rtyp=INT_CMD;
    result.get()->data=(void*)neg((long)data);
    return result;
  }

  // The interpreter dispatches an overloaded unary operator before assigning
  // its result to a typed destination.  Work on a private copy because the
  // ordinary dispatcher may consume or normalize its input.
  Value input(r);
  sleftv head=*argument;
  head.next=nullptr;
  input.get()->Copy(&head);
  if (errorreported) throw ReportedInterpreterError();
  struct RestoreOperation {
    int saved;
    RestoreOperation(): saved(iiOp) {}
    ~RestoreOperation() { iiOp=saved; }
  } restore_operation;
  const BOOLEAN failed=iiExprArith1(result.get(),input.get(),operation);
  if (errorreported) throw ReportedInterpreterError();
  if (failed)
    throw std::runtime_error("native dynamic unary operation failed");
  return result;
}
inline intvec *intvec_argument(leftv argument) {
  argument=value_argument(argument);
  if (argument->Typ()!=INTVEC_CMD || metadata(argument))
    throw std::runtime_error("native value requires a plain intvec");
  intvec *value=(intvec*)argument->Data();
  if (errorreported || value==nullptr)
    throw std::runtime_error("native intvec argument is invalid");
  return value;
}
class IntvecArgument {
  Intvec converted;
  intvec *value;
  bool vector_value;
public:
  explicit IntvecArgument(leftv argument): converted((intvec*)nullptr), value(nullptr), vector_value(false) {
    sleftv materialized;
    char string_value[2];
    argument=scalar_index_argument(argument,materialized,string_value);
    argument=value_argument(argument);
    if (metadata(argument))
      throw std::runtime_error("native intvec argument must be a plain intvec or int");
    if (argument->Typ()==INTVEC_CMD) {
      vector_value=true;
      value=(intvec*)argument->Data();
      if (errorreported || value==nullptr)
        throw std::runtime_error("native intvec argument is invalid");
    } else if (argument->Typ()==INT_CMD) {
      void *data=argument->Data();
      if (errorreported) throw std::runtime_error("native int argument is invalid");
      converted=Intvec(1);
      (*converted.get())[0]=(int)(long)data;
      value=converted.get();
    } else {
      throw std::runtime_error("native intvec argument requires intvec or int");
    }
  }
  IntvecArgument(const IntvecArgument &) = delete;
  IntvecArgument &operator=(const IntvecArgument &) = delete;
  intvec *get() const { return value; }
  bool is_intvec() const { return vector_value; }
};
class StringArgument {
  sleftv materialized;
  char string_value[2];
  const char *value;
public:
  explicit StringArgument(leftv argument): value(nullptr) {
    argument=scalar_index_argument(argument,materialized,string_value);
    argument=value_argument(argument);
    if (argument->Typ()!=STRING_CMD || metadata(argument))
      throw std::runtime_error("native string argument requires a plain string");
    value=(const char*)argument->Data();
    if (errorreported) throw std::runtime_error("native string argument is invalid");
    if (value==nullptr) value="";
  }
  const char *get() const { return value; }
};
inline String exact_string_argument(leftv argument) {
  StringArgument value(argument);
  return String::copy(value.get());
}
class IdealArgument {
  Ideal converted;
  IdealView value;
public:
  IdealArgument(leftv argument, ring r): converted((ideal)nullptr,r), value(nullptr,0) {
    sleftv materialized;
    char string_value[2];
    argument=scalar_index_argument(argument,materialized,string_value);
    argument=value_argument(argument);
    if (attributed(argument))
      throw std::runtime_error("native ideal argument must be a plain ideal, poly, number, or int");
    const int type=argument->Typ();
    if (type==IDEAL_CMD) {
      ideal data=(ideal)argument->Data();
      if (errorreported || data==nullptr)
        throw std::runtime_error("native ideal argument is invalid");
      value=IdealView(data,value_flags(argument));
    } else if (type==POLY_CMD || type==BUCKET_CMD) {
      converted=ideal_from_poly(borrow_poly(argument),r);
      value=converted.get();
    } else if (type==INT_CMD) {
      void *data=argument->Data();
      if (errorreported) throw std::runtime_error("native int argument is invalid");
      converted=ideal_from_int((long)data,r);
      value=converted.get();
    } else if (type==NUMBER_CMD) {
      number data=(number)argument->Data();
      if (errorreported) throw std::runtime_error("native number argument is invalid");
      converted=ideal_from_number(data,r);
      value=converted.get();
    } else {
      throw std::runtime_error("native ideal argument requires ideal, poly, number, or int");
    }
  }
  IdealArgument(const IdealArgument &) = delete;
  IdealArgument &operator=(const IdealArgument &) = delete;
  IdealView get() const { return value; }
};
inline Ideal exact_ideal_argument(leftv argument, ring r) {
  argument=value_argument(argument);
  if (argument->Typ()!=IDEAL_CMD || attributed(argument))
    throw std::runtime_error("native ideal return requires a plain ideal");
  ideal value=(ideal)argument->Data();
  if (errorreported || value==nullptr)
    throw std::runtime_error("native ideal return is invalid");
  return Ideal::copy(IdealView(value,value_flags(argument)),r);
}
inline Number exact_number_argument(leftv argument, ring r);
inline Poly exact_poly_argument(leftv argument, ring r) {
  argument=value_argument(argument);
  if ((argument->Typ()!=POLY_CMD && argument->Typ()!=BUCKET_CMD) ||
      unsupported_poly_result_metadata(argument))
    throw std::runtime_error("native polynomial result requires a plain poly");
  poly value=borrow_poly(argument);
  if (errorreported) throw ReportedInterpreterError();
  return Poly(p_Copy(value,r),r);
}
inline Value binary_static_value(int type, void *data, BITSET flags,
                                 leftv argument, int operation, ring r) {
  Value left(r), right(r), result(r);
  left.get()->rtyp=type;
  if (type==POLY_CMD)
    left.get()->data=(void*)p_Copy((poly)data,r);
  else if (type==NUMBER_CMD)
    left.get()->data=(void*)n_Copy((number)data,r->cf);
  else if (type==IDEAL_CMD) {
    left.get()->data=(void*)id_Copy((ideal)data,r);
    left.get()->flag=flags;
  } else {
    throw std::runtime_error("unsupported native static/dynamic operation");
  }
  sleftv head=*argument;
  head.next=nullptr;
  right.get()->Copy(&head);
  if (errorreported) throw ReportedInterpreterError();
  struct RestoreOperation {
    int saved;
    RestoreOperation(): saved(iiOp) {}
    ~RestoreOperation() { iiOp=saved; }
  } restore_operation;
  const BOOLEAN failed=iiExprArith2(result.get(),left.get(),operation,right.get());
  if (errorreported) throw ReportedInterpreterError();
  if (failed)
    throw std::runtime_error("native static/dynamic operation failed");
  return result;
}
inline Poly poly_power_value(poly base, leftv exponent, ring r) {
  sleftv materialized;
  char string_value[2];
  exponent=scalar_index_argument(exponent,materialized,string_value);
  exponent=value_argument(exponent);
  if (exponent->Typ()==INT_CMD)
    return power(Poly::copy(base,r),power_exponent(exponent));
  Value result=binary_static_value(POLY_CMD,(void*)base,0,exponent,'^',r);
  return exact_poly_argument(result.get(),r);
}
inline Number number_power_value(number base, leftv exponent, ring r) {
  sleftv materialized;
  char string_value[2];
  exponent=scalar_index_argument(exponent,materialized,string_value);
  exponent=value_argument(exponent);
  if (exponent->Typ()==INT_CMD)
    return number_power(base,power_exponent(exponent),r);
  Value result=binary_static_value(NUMBER_CMD,(void*)base,0,exponent,'^',r);
  return exact_number_argument(result.get(),r);
}
inline Ideal ideal_power_value(IdealView base, leftv exponent, ring r) {
  sleftv materialized;
  char string_value[2];
  exponent=scalar_index_argument(exponent,materialized,string_value);
  exponent=value_argument(exponent);
  if (exponent->Typ()==INT_CMD)
    return ideal_power(base,power_exponent(exponent),r);
  Value result=binary_static_value(
      IDEAL_CMD,(void*)base.data(),base.flags(),exponent,'^',r);
  return exact_ideal_argument(result.get(),r);
}
inline Value indexed_value(leftv argument, int index, ring r) {
  argument=value_argument(argument);
  Value result(r);
  if (argument->Typ()==LIST_CMD) {
    result.get()->Copy(get(list_argument(argument),index));
  } else if (argument->Typ()==INTVEC_CMD) {
    intvec *v=(intvec*)argument->Data();
    if (index<1 || index>v->length()) throw std::runtime_error("native intvec index out of range");
    result.get()->rtyp=INT_CMD;
    result.get()->data=(void*)(long)(*v)[index-1];
  } else if (argument->Typ()==IDEAL_CMD || argument->Typ()==MODUL_CMD ||
             argument->Typ()==MAP_CMD) {
    const int type=argument->Typ();
    ideal value=(ideal)argument->Data();
    if (errorreported || value==nullptr || index<1 || index>IDELEMS(value))
      throw std::runtime_error("native ideal/module index out of range");
    result.get()->rtyp=type==MODUL_CMD ? VECTOR_CMD : POLY_CMD;
    result.get()->data=(void*)p_Copy(value->m[index-1],r);
  } else if (argument->Typ()==VECTOR_CMD) {
    poly value=(poly)argument->Data();
    if (errorreported)
      throw std::runtime_error("native vector argument is invalid");
    result.get()->rtyp=POLY_CMD;
    // Match jjINDEX_V: select and copy one component, stripping its vector
    // component marker from the returned polynomial.
    result.get()->data=(void*)p_Vec2Poly(value,index,r);
  } else if (argument->Typ()==STRING_CMD) {
    const char *value=(const char*)argument->Data();
    if (errorreported) throw std::runtime_error("native string argument is invalid");
    char *selected=(char*)omAlloc(2);
    selected[0]=(value!=nullptr && index>0 && index<=(long)strlen(value))
                  ? value[index-1] : '\0';
    selected[1]='\0';
    result.get()->rtyp=STRING_CMD;
    result.get()->data=(void*)selected;
  } else {
    throw std::runtime_error("native indexing requires list/intvec/string/ideal/module/map/vector");
  }
  return result;
}
inline String type_name(leftv argument) {
  argument=value_argument(argument);
  return String::copy(Tok2Cmdname(argument->Typ()));
}
inline bool type_is(leftv argument, const char *name) {
  // typeof comparisons only need the stable token name; avoid constructing a
  // temporary std::string on every test (notably in recursive procedures).
  argument=value_argument(argument);
  const char *actual=Tok2Cmdname(argument->Typ());
  return actual!=nullptr && name!=nullptr && strcmp(actual,name)==0;
}
inline List list_of(std::initializer_list<leftv> elements, ring r) {
  lists l=(lists)omAlloc0Bin(slists_bin);
  l->Init((int)elements.size());
  List owned(l,r);
  int i=0;
  for (leftv element: elements) {
    sleftv view=*value_argument(element); view.next=nullptr;
    l->m[i++].Copy(&view);
  }
  return owned;
}
inline List list_delete(lists l, long index, ring r) {
  if (index<1 || index>l->nr+1 || index>INT_MAX)
    throw std::runtime_error("native list delete index out of range");
  // lDelete assumes no trailing unassigned slots. Reject that unsupported
  // case, avoiding its out-of-bounds transfer on sparse lists.
  if (lSize(l)!=l->nr)
    throw std::runtime_error("native delete requires no trailing undefined list slots");
  View borrowed(LIST_CMD,l), at(INT_CMD,(void*)index);
  Value input(r), result(r);
  input.get()->Copy(borrowed.get());
  result.get()->rtyp=LIST_CMD;
  // lDelete's implementation calls CopyD through its leftv.  A plain View has
  // no ownership boundary and can therefore expose/move the caller's list.
  if (lDelete(result.get(),input.get(),at.get())) throw std::runtime_error("native list delete failed");
  lists output=(lists)result.get()->data; result.get()->Init();
  return List(output,r);
}
inline List list_add(lists a, lists b, ring r) {
  // Same element data/type result as lAdd. That kernel deep-copies attributes
  // only to discard their pointers; copying data directly avoids that leak.
  lists output=(lists)omAlloc0Bin(slists_bin);
  output->Init(a->nr+b->nr+2);
  List result(output,r);
  int at=0;
  for (lists input: {a,b}) for (int i=0;i<=input->nr;++i) {
    Value element(r); element.get()->Copy(&input->m[i]);
    output->m[at].rtyp=element.get()->rtyp;
    output->m[at++].data=element.get()->data;
    element.get()->data=nullptr; // its discarded metadata is still cleaned
  }
  return result;
}
inline void list_discard_metadata(sleftv &entry, ring r) {
  // lAdd retains only type and data from each copied element.  When the left
  // list is already compiler-owned, detach those fields while cleaning the
  // same name/attribute/flag/index metadata that lAdd would discard.
  const int type=entry.rtyp;
  void *data=entry.data;
  entry.data=nullptr;
  entry.CleanUp(r);
  entry.rtyp=type;
  entry.data=data;
}
inline List list_add(List a, lists b, ring r) {
  lists left=a.get();
  if (left==nullptr || b==nullptr)
    throw std::runtime_error("native list concatenation requires lists");
  // Generated code copies an aliased RHS before moving the left owner. Keep a
  // defensive path for direct callers which already evaluated both arguments.
  if (left==b) {
    List rhs=List::copy(b,r);
    return list_add(std::move(a),rhs.get(),r);
  }
  const long left_size=(long)left->nr+1;
  const long right_size=(long)b->nr+1;
  if (left_size>INT_MAX-right_size)
    throw std::runtime_error("native list capacity out of range");
  for (int i=0;i<=left->nr;++i) list_discard_metadata(left->m[i],r);
  const long total=left_size+right_size;
  a.reserve(total);
  left->nr=(int)total-1; // make every appended value visible to RAII cleanup
  for (long i=0;i<right_size;++i) {
    Value element(r); element.get()->Copy(&b->m[i]);
    left->m[left_size+i].rtyp=element.get()->rtyp;
    left->m[left_size+i].data=element.get()->data;
    element.get()->data=nullptr; // discard copied metadata, as lAdd does
  }
  return a;
}
inline leftv list_assignment_slot(List &target, int index, ring r) {
  if (index<1) throw std::runtime_error("native list assignment index out of range");
  lists l=target.get();
  if (index>l->nr+1) {
    int old=l->nr+1;
    target.reserve(index);
    for (int i=old;i<index;++i) l->m[i].rtyp=DEF_CMD;
    l->nr=index-1;
  }
  l->m[index-1].CleanUp(r);
  return &l->m[index-1];
}
inline void list_set(List &target, int index, leftv value, ring r) {
  // Copy before resizing/cleaning: the RHS may refer into this same list.
  Value copied(r);
  sleftv materialized;
  char string_value[2];
  value=scalar_index_argument(value,materialized,string_value);
  sleftv view=*value_argument(value); view.next=nullptr;
  copied.get()->Copy(&view);
  leftv slot=list_assignment_slot(target,index,r);
  *slot=*copied.get(); copied.get()->Init();
}
inline void list_set_argument(List &target, int index, leftv value, ring r) {
  const int type=value==nullptr ? 0 : value->Typ();
  if (type==0 || type==NONE || type==DEF_CMD)
    throw std::runtime_error("native variadic argument is undefined");
  list_set(target,index,value,r);
}
inline void list_set_owned(List &target, int index, List &&value, ring r) {
  // A compiler-owned list temporary has no interpreter-level aliases, so its
  // storage can become the slot value directly. Keep the self-move case safe
  // for hand-written/generated callers by falling back to value semantics.
  if (&target==&value) {
    View borrowed(LIST_CMD,value.get());
    list_set(target,index,borrowed.get(),r);
    return;
  }
  leftv slot=list_assignment_slot(target,index,r);
  slot->rtyp=LIST_CMD;
  slot->data=value.release();
}
inline void list_set_owned(List &target, int index, Poly &&value, ring r) {
  leftv slot=list_assignment_slot(target,index,r);
  slot->rtyp=POLY_CMD;
  slot->data=value.release();
}
inline void list_set_owned(List &target, int index, Number &&value, ring r) {
  leftv slot=list_assignment_slot(target,index,r);
  slot->rtyp=NUMBER_CMD;
  slot->data=value.release();
}
inline void list_set_owned(List &target, int index, String &&value, ring r) {
  leftv slot=list_assignment_slot(target,index,r);
  slot->rtyp=STRING_CMD;
  slot->data=value.release();
}
inline void list_set_owned(List &target, int index, Ideal &&value, ring r) {
  leftv slot=list_assignment_slot(target,index,r);
  slot->rtyp=IDEAL_CMD;
  slot->flag=value.flags();
  slot->data=value.release();
}
inline void list_set_owned(List &target, int index, Intvec &&value, ring r) {
  leftv slot=list_assignment_slot(target,index,r);
  slot->rtyp=INTVEC_CMD;
  slot->data=value.release();
}
inline List arguments_list(leftv arguments, ring r) {
  List result(r);
  int index=1;
  for (leftv argument=arguments; argument!=nullptr; argument=argument->next) {
    const int type=argument->Typ();
    if (type==0 || type==NONE || type==DEF_CMD)
      throw std::runtime_error("native variadic argument is undefined");
    list_set(result,index++,argument,r);
  }
  return result;
}
inline List list_insert(lists input, leftv value, long position, ring r) {
  List copied=List::copy(input,r);
  // lInsert0 consumes its value through CopyD().  Keep procedure arguments
  // borrowed by giving that destructive kernel a private copy to move from.
  Value inserted(r);
  sleftv inserted_view=*value_argument(value);
  inserted_view.next=nullptr;
  inserted.get()->Copy(&inserted_view);
  lists raw=copied.release();
  lists result=lInsert0(raw,inserted.get(),(int)position);
  if (result==nullptr) {
    List cleanup(raw,r);
    throw std::runtime_error("native list insert failed");
  }
  return List(result,r);
}
inline Value struct_field(leftv argument, const char *name, ring r) {
  argument=value_argument(argument);
  blackbox *box=getBlackboxStuff(argument->Typ());
  if (box==nullptr || box->blackbox_Op2!=newstruct_Op2 || box->data==nullptr)
    throw std::runtime_error("native field access requires a newstruct value");
  // The '.' branch of this exact kernel never invokes overloaded/user procs.
  // It updates shadow-ring metadata, so operate on a private value.
  Value input(r), member(r), selected(r), result(r);
  sleftv view=*argument; view.next=nullptr;
  input.get()->Copy(&view);
  member.get()->name=omStrDup(name);
  if (newstruct_Op2('.',selected.get(),input.get(),member.get()))
    throw std::runtime_error("native newstruct member lookup failed");
  result.get()->Copy(selected.get());
  if (errorreported) throw std::runtime_error("native newstruct member copy failed");
  return result;
}
inline List parameter_names(ring r) {
  List description(rDecompose(r),r);
  if (description.get()==nullptr) throw std::runtime_error("native ringlist failed");
  lists coefficient=list_argument(get(description.get(),1));
  return List::copy(list_argument(get(coefficient,2)),r);
}
inline Number parameter_number(leftv name, ring r, std::initializer_list<const char*> locals) {
  if (name->Typ()!=STRING_CMD) throw std::runtime_error("native parameter name requires string");
  const char *text=(const char*)name->Data();
  // execute would resolve local/global identifiers first. Reject native-local
  // shadows rather than silently reading a different coefficient parameter.
  for (const char *local: locals)
    if (strcmp(text,local)==0) throw std::runtime_error("native coefficient parameter is shadowed by a local");
  if (ggetid(text)!=nullptr)
    throw std::runtime_error("native coefficient parameter is shadowed by an identifier");
  for (int i=0;i<n_NumberOfParameters(r->cf);++i)
    if (strcmp(text,n_ParameterNames(r->cf)[i])==0)
      return Number(n_Param(i+1,r->cf),r->cf);
  throw std::runtime_error("native parameter name is not a coefficient parameter");
}
inline long remainder(long a, long b) {
  if (b==0) throw std::runtime_error("div. by 0");
  return a==LONG_MIN && b==-1 ? 0 : a%b;
}
inline long quotient(long a, long b) {
  if (b==0) throw std::runtime_error("div. by 0");
  if (a==LONG_MIN && b==-1) return LONG_MIN;
  const long r=a%b;
  return (a-r)/b;
}
inline Value binary_value_int(leftv argument, long integer, int operation,
                              bool value_on_left, ring r) {
  sleftv materialized;
  char string_value[2];
  argument=scalar_index_argument(argument,materialized,string_value);
  argument=value_argument(argument);
  const int argument_type=argument->Typ();
  if (errorreported) throw ReportedInterpreterError();

  Value result(r);
  if (argument_type==INT_CMD) {
    void *data=argument->Data();
    if (errorreported) throw ReportedInterpreterError();
    const long dynamic=(long)data;
    const long a=value_on_left ? dynamic : integer;
    const long b=value_on_left ? integer : dynamic;
    long value;
    switch (operation) {
      case '+': value=add(a,b); break;
      case '-': value=sub(a,b); break;
      case '*': value=mul(a,b); break;
      case '%': value=remainder(a,b); break;
      case INTDIV_CMD: value=quotient(a,b); break;
      case '^': value=integer_power(a,b); break;
      default: throw std::runtime_error("unsupported native dynamic integer operation");
    }
    result.get()->rtyp=INT_CMD;
    result.get()->data=(void*)value;
    return result;
  }

  Value left(r), right(r);
  leftv dynamic=value_on_left ? left.get() : right.get();
  leftv scalar=value_on_left ? right.get() : left.get();
  sleftv head=*argument;
  head.next=nullptr;
  dynamic->Copy(&head);
  if (errorreported) throw ReportedInterpreterError();
  scalar->rtyp=INT_CMD;
  scalar->data=(void*)integer;
  struct RestoreOperation {
    int saved;
    RestoreOperation(): saved(iiOp) {}
    ~RestoreOperation() { iiOp=saved; }
  } restore_operation;
  const BOOLEAN failed=iiExprArith2(result.get(),left.get(),operation,right.get());
  if (errorreported) throw ReportedInterpreterError();
  if (failed)
    throw std::runtime_error("native dynamic integer operation failed");
  return result;
}
inline long number_to_int(number value, ring r) {
  number copy=n_Copy(value,r->cf); // n_Int may normalize destructively
  const long wide=n_Int(copy,r->cf);
  n_Delete(&copy,r->cf);
  const int narrowed=(int)wide;
  return (long)narrowed==wide ? wide : 0;
}
inline void *checked_number_argument(leftv &argument, sleftv &materialized) {
  // The only implicit conversions to a procedure's number parameter are the
  // exact number and int cases. In particular poly -> number is explicit only.
  char string_value[2];
  argument=scalar_index_argument(argument,materialized,string_value);
  argument=list_element_argument(argument);
  if (argument==nullptr || argument->e!=nullptr ||
      (argument->rtyp!=NUMBER_CMD && argument->rtyp!=INT_CMD && argument->rtyp!=IDHDL) || metadata(argument))
    throw std::runtime_error("native number argument must be a plain number or int");
  const int type=argument->Typ();
  void *data=argument->Data();
  if (errorreported) throw std::runtime_error("native number argument is invalid");
  if (type==NUMBER_CMD || type==INT_CMD) return data;
  throw std::runtime_error("native number argument requires number or int");
}
inline Number number_argument(leftv argument, ring r) {
  sleftv materialized;
  void *data=checked_number_argument(argument,materialized);
  if (argument->Typ()==NUMBER_CMD) return Number::copy((number)data,r);
  return number_from_int((long)data,r);
}
inline Number exact_number_argument(leftv argument, ring r) {
  // A dynamic return has no typed-number assignment context.  Unlike a
  // declared number parameter, it must not silently coerce an int branch.
  sleftv materialized;
  char string_value[2];
  argument=scalar_index_argument(argument,materialized,string_value);
  argument=value_argument(argument);
  if (argument->Typ()!=NUMBER_CMD || metadata(argument))
    throw std::runtime_error("native number return requires a plain number");
  number value=(number)argument->Data();
  if (errorreported) throw std::runtime_error("native number return is invalid");
  return Number::copy(value,r);
}
class NumberArgument {
  Number converted;
  number value;
public:
  NumberArgument(leftv argument, ring r): converted(nullptr,r->cf), value(nullptr) {
    sleftv materialized;
    void *data=checked_number_argument(argument,materialized);
    if (argument->Typ()==NUMBER_CMD) {
      // Entry arguments outlive the native call, so exact numbers are views.
      value=(number)data;
    } else {
      // Singular's implicit int-to-number conversion does need owned storage.
      converted=number_from_int((long)data,r);
      value=converted.get();
    }
  }
  NumberArgument(const NumberArgument &) = delete;
  NumberArgument &operator=(const NumberArgument &) = delete;
  number get() const { return value; }
};
inline Number indexed_number(leftv argument, int index, ring r) {
  // Contextual number lowering can copy/convert the selected scalar exactly
  // once instead of first materializing a generic owning Value.
  argument=value_argument(argument);
  if (argument->Typ()==LIST_CMD)
    return number_argument(get(list_argument(argument),index),r);
  if (argument->Typ()==INTVEC_CMD) {
    intvec *v=(intvec*)argument->Data();
    if (errorreported || v==nullptr)
      throw std::runtime_error("native intvec argument is invalid");
    if (index<1 || index>v->length())
      throw std::runtime_error("native intvec index out of range");
    return number_from_int((long)(*v)[index-1],r);
  }
  throw std::runtime_error("native indexing requires list or intvec");
}

inline Intvec intvec_range(long first, long last) {
  return Intvec(new intvec((int)first,(int)last));
}
inline Intvec intvec_repeat(long value, long count) {
  const int narrowed_count=(int)count;
  if (narrowed_count<0)
    throw std::runtime_error("native intvec repetition count out of range");
  Intvec result(narrowed_count);
  for (int i=0;i<narrowed_count;++i) (*result.get())[i]=(int)value;
  return result;
}
inline Intvec intvec_select(const intvec *source, const intvec *indexes) {
  // jjINDEX_IV leaves the dispatcher's default scalar zero in place when its
  // selector is empty; contextual assignment to intvec therefore yields [0].
  Intvec result(indexes->length()==0 ? 1 : indexes->length());
  for (int i=0;i<indexes->length();++i) {
    const long index=(*indexes)[i];
    if (index<=0 || index>source->length())
      throw std::runtime_error("native intvec index out of range");
    (*result.get())[i]=(*source)[(int)index-1];
  }
  return result;
}
inline Intvec intvec_select_argument(const intvec *source, leftv indexes,
                                     bool vector_source_is_named) {
  // In an explicit intvec destination, a dynamic selector has two exact
  // interpreter cases: INT selects one element and INTVEC selects many.
  IntvecArgument converted(indexes);
  if (converted.is_intvec() && !vector_source_is_named)
    throw std::runtime_error("vector-valued indexing requires a named source");
  return intvec_select(source,converted.get());
}
inline Intvec intvec_delete(const intvec *source, long index) {
  // intvec::delete_pos predates const-correctness but only reads its receiver.
  intvec *result=const_cast<intvec *>(source)->delete_pos((int)index-1);
  if (result==nullptr) throw std::runtime_error("native intvec delete index out of range");
  return Intvec(result);
}
inline void append(Intvec &target, long value) {
  intvec *v=target.get();
  const int old=v->length();
  if (old==INT_MAX) throw std::runtime_error("native intvec capacity out of range");
  v->resize(old+1);
  (*v)[old]=(int)value;
}
inline void append(Intvec &target, const intvec *value) {
  intvec *v=target.get();
  const int old=v->length(), extra=value->length();
  if (extra>INT_MAX-old) throw std::runtime_error("native intvec capacity out of range");
  v->resize(old+extra);
  for (int i=0;i<extra;++i) (*v)[old+i]=(*value)[i];
}
inline Intvec intvec_binary(const intvec *a, const intvec *b, bool subtract) {
  intvec *result=subtract ? ivSub((intvec*)a,(intvec*)b) : ivAdd((intvec*)a,(intvec*)b);
  if (result==nullptr) throw std::runtime_error("intvec size not compatible");
  return Intvec(result);
}
inline Intvec intvec_scalar(const intvec *a, long b, int operation) {
  Intvec result=Intvec::copy(a);
  const int value=(int)b;
  switch (operation) {
    case '+': (*result.get())+=value; break;
    case '-': (*result.get())-=value; break;
    case '*': (*result.get())*=value; break;
    case '/': (*result.get())/=value; break;
    case '%': (*result.get())%=value; break;
    default: throw std::runtime_error("unsupported native intvec operation");
  }
  return result;
}
inline Intvec intvec_neg(const intvec *a) { return intvec_scalar(a,-1,'*'); }
inline long intvec_compare(const intvec *a, const intvec *b, int operation) {
  const int comparison=a->compare(b);
  if (comparison==-2) throw std::runtime_error("intvec size incompatible");
  switch (operation) {
    case '<': return comparison<0;
    case '>': return comparison>0;
    case 1: return comparison<=0;
    case 2: return comparison>=0;
    case 3: return comparison==0;
    case 4: return comparison!=0;
  }
  throw std::runtime_error("unsupported native intvec comparison");
}
inline long intvec_compare(const intvec *a, long b, int operation) {
  const int comparison=a->compare((int)b);
  switch (operation) {
    case '<': return comparison<0;
    case '>': return comparison>0;
    case 1: return comparison<=0;
    case 2: return comparison>=0;
    case 3: return comparison==0;
    case 4: return comparison!=0;
  }
  throw std::runtime_error("unsupported native intvec comparison");
}

// Singular intvec indexing is one-based and grows on assignment.
inline void set(Intvec &owner, int index, long value) {
  intvec &v=*owner.get();
  if (index<=0)
    throw std::runtime_error("native intvec index out of range");
  if (index>v.length()) v.resize(index);
  v[index-1]=(int)value;
}
inline long get(const intvec *v, int index) {
  if (v==nullptr || index<=0 || index>v->length())
    throw std::runtime_error("native intvec index out of range");
  return (*v)[index-1];
}

// Match jjDEG and jjDEG_IV, including zero-polynomial and ring-order rules.
inline long degree(poly p, const ring r) {
  int dummy;
  return p==NULL ? -1 : r->pLDeg(p,&dummy,r);
}
inline long degree(poly p, intvec *weights, const ring r) {
  if (p==NULL) return -1;
  int *w=iv2array(weights,r);
  const long result=p_DegW(p,w,r);
  omFreeSize(w,(rVar(r)+1)*sizeof(int));
  return result;
}
}
#endif
