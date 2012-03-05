#ifndef POLYCPP_HEADER
#define POLYCPP_HEADER
#include <kernel/mod2.h>
#include <kernel/IIntvec.h>
#include <kernel/numbers.h>
#include <kernel/Number.h>
#include <kernel/febase.h>
#include <kernel/polys.h>
#include <kernel/ring.h>


#include <boost/shared_ptr.hpp>

#include <vector>
#include <exception>
using std::exception;

#define BOOST_DISABLE_THREADS

class DifferentDomainException: public exception{

};
class ExceptionBasedErrorHandler{
    public:
        static const bool handleErrors=true;
        static void handleDifferentRing(ring r, ring s){
        PrintS("throwing");
        throw DifferentDomainException();
    }
};

//PolyImpl is a 08/15 poly wrapper
//Poly wraps around PolyImpl with reference counting using boost
class TrivialErrorHandler{
    public:
    static const bool handleErrors=false;
    static void handleDifferentRing(ring r, ring s){
    }
};
typedef TrivialErrorHandler MyErrorHandler;
class PolyImpl{
  template <poly_variant,class,class> friend class PolyBase;
  //friend class PolyBase<POLY_VARIANT_RING,Poly,TrivialErrorHandler>;
  //friend class PolyBase<POLY_VARIANT_MODUL,Vector, TrivialErrorHandler>;
  //friend class PolyBase<POLY_VARIANT_MODUL>;
  friend class Poly;
  friend class Vector;
  //friend class Number;
 protected:
  poly getInternalReference() const{
    return p;
  }
 public:
  ring getRing() const{
    return r.get();
  }
  friend inline bool operator==(const Poly& p1, const Poly& p2);
  friend inline bool operator==(const Vector& p1, const Vector& p2);
  friend PolyImpl operator+(const PolyImpl& p1, const PolyImpl& n2);
  friend PolyImpl operator-(const PolyImpl& p1, const PolyImpl& n2);
  friend PolyImpl operator/(const PolyImpl& p1, const PolyImpl& n2);
  friend PolyImpl operator*(const PolyImpl& p1, const PolyImpl& n2);
  friend bool operator==(const PolyImpl& p1, const PolyImpl& n2);
  friend PolyImpl operator+(const PolyImpl& p1, int n2);
  friend PolyImpl operator-(const PolyImpl& p1, int n2);
  friend PolyImpl operator/(const PolyImpl& p1, int n2);
  friend PolyImpl operator*(const PolyImpl& p1, int n2);
  friend bool operator==(const PolyImpl& p1, int n2);
  Number leadCoef(){
    return Number(p->coef,r.get());
  }
  PolyImpl& operator=(const PolyImpl& p2){
    //durch Reihenfolge Selbstzuweisungen berücksichtigt
    if (this==&p2) return *this;
    poly pc=p_Copy(p2.p,p2.r.get());
    if(r!=NULL)
      p_Delete(&p,r.get());
    r=p2.r;
    p=pc;
    return *this;
  }
  PolyImpl operator-(){
    PolyImpl t(*this);
    t.p=p_Copy(p,r.get());
    t.p=p_Neg(t.p,r.get());
    return t;
  }
  PolyImpl& operator+=(const PolyImpl & p2){
    if (r!=p2.r){
      Werror("not the same ring");
      return *this;
    }
    if (this==&p2){
      number two=n_Init(2,r.get());
      p_Mult_nn(p,two,r.get());
      return *this;
    }
    p=p_Add_q(p,p_Copy(p2.p,p2.r.get()),r.get());

    return *this;
  }
  PolyImpl& operator*=(const PolyImpl & p2){
    if (r!=p2.r){
      Werror("not the same ring");
      return *this;
    }
    if (this==&p2){
      poly pc=p_Copy(p,r.get());
      p=p_Mult_q(p,p2.p,r.get());
      return *this;
    }
    p=p_Mult_q(p,p_Copy(p2.p,p2.r.get()),r.get());
    return *this;
  }
  PolyImpl& operator*=(const Number & n){
    if (r!=n.r){
      Werror("not the same ring");
      return *this;
    }

    p=p_Mult_nn(p,n.n,r.get());
    return *this;
  }
  PolyImpl& operator-=(const PolyImpl & p2){
    if (r!=p2.r){
      Werror("not the same ring");
      return *this;
    }
    if (this==&p2){
      p_Delete(&p,r.get());
      p=NULL;
      return *this;
    }

    poly pc=p_Copy(p2.p,p2.r.get());
    pc=p_Neg(pc,r.get());
    p=p_Add_q(p,pc,r.get());


    return *this;
  }


  PolyImpl& operator=(int n){

    p_Delete(&p,r.get());
    p=p_ISet(n,r.get());
    return *this;

  }


  PolyImpl(){
    r=currRing;
    p=NULL;
  }
  PolyImpl(const PolyImpl & p){
    r=p.r;
    this->p=p_Copy(p.p,r.get());
  }
  PolyImpl(poly p, intrusive_ptr<ip_sring> r){
    this->p=p_Copy(p,r.get());
    this->r=r;
  }
  PolyImpl(poly p, intrusive_ptr<ip_sring> r,int){
    this->p=p;
    this->r=r;
  }
  PolyImpl(int n, intrusive_ptr<ip_sring> r){
    this->p=p_ISet(n,r.get());
    this->r=r;
  }
  PolyImpl(const Number & n){

    r=n.r.get();
    this->p=p_NSet(n_Copy(n.n,r.get()),r.get());

  }
  explicit PolyImpl(int n){
    r=currRing;
    this->p=p_ISet(n,r.get());
  }
  void print(){
    p_Write(p,r.get(),r.get());
  }

  virtual ~PolyImpl(){
    if (r!=NULL)
      p_Delete(&p,r.get());
  }

 protected:
  poly p;
  intrusive_ptr<ip_sring> r;

};

inline PolyImpl operator+(const PolyImpl &p1, const PolyImpl& p2){
  PolyImpl erg(p1);
  erg+=p2;
  return erg;
}
inline PolyImpl operator*(const PolyImpl &p1, const PolyImpl& p2){
  PolyImpl erg(p1);
  erg*=p2;
  return erg;
}
inline PolyImpl operator-(const PolyImpl &p1, const PolyImpl& p2){
  PolyImpl erg(p1);
  erg-=p2;
  return erg;
}



inline PolyImpl operator+(const PolyImpl &p1, int p2){
  PolyImpl erg(p1);
  erg+=PolyImpl(p2,p1.r.get());
  return erg;
}
inline PolyImpl operator*(const PolyImpl &p1, int p2){
  PolyImpl erg(p1);
  erg*=PolyImpl(p2,p1.r.get());
  return erg;
}
inline PolyImpl operator-(const PolyImpl &p1, int p2){
  PolyImpl erg(p1);
  erg-=PolyImpl(p2,p1.r);
  return erg;
}


inline PolyImpl operator+(int p1, const PolyImpl& p2){
  PolyImpl erg(p2);
  return erg+=PolyImpl(p1,p2.getRing());
}

inline PolyImpl operator*(int p1, const PolyImpl& p2){
  PolyImpl erg(p2);
  return erg*=PolyImpl(p1,p2.getRing());
}

using namespace boost;


template<class T> class ConstTermReference{
 private:
  ring r;
  poly t;
 public:
  operator T() const {
    return T(p_Head(t,r),r);
  }
  ConstTermReference(poly p, ring r){
    this->t=p;
    this->r=r;
  }
  bool isConstant() const{
    return p_LmIsConstant(t,r);
  }

};

template<class T> class PolyInputIterator:
public std::iterator<std::input_iterator_tag,T,int, shared_ptr<const T>,ConstTermReference<T> >
{


 private:
  poly t;
  ring r;
  public:
  bool operator==(const PolyInputIterator& t2){
    return t2.t==t;
  }
  bool operator!=(const PolyInputIterator& t2){
    return t2.t!=t;
  }
  PolyInputIterator(poly p, ring r){
    t=p;
    this->r=r;
  }
  PolyInputIterator(const PolyInputIterator& it){
    t=it.t;
    r=it.r;
  }
  PolyInputIterator& operator++(){
    t=t->next;
    return *this;
  }
  PolyInputIterator operator++(int){
    PolyInputIterator it(*this);
    ++(*this);
    return it;
  }
  const ConstTermReference<T> operator*(){
    return ConstTermReference<T> (t,r);
  }
  shared_ptr<const T> operator->(){
    return shared_ptr<const T>(new T(p_Head(t,r),r,0));
  }

};

template<poly_variant variant, class create_type_input, class error_handle_traits> class PolyBase{
 private:
    typedef PolyBase<variant,create_type_input,error_handle_traits> ThisType;
 public:
  poly as_poly() const{
    return p_Copy(ptr->p,ptr->getRing());
  }
  template<class T> void checkIsSameRing(T& p){
    if (error_handle_traits::handleErrors){
                 if (p.getRing()!=this->getRing()){
   error_handle_traits::handleDifferentRing(
    this->getRing(),
    p.getRing()
   );
        }
    }
  }
  typedef create_type_input create_type;
  typedef PolyInputIterator<create_type> iterator;
  Intvec leadExp(){
    int nvars=rVar(ptr->r);
    Intvec res(nvars);
    for(int i=0;i<nvars;i++){
      res[i]=p_GetExp(ptr->p,i+1,ptr->getRing());
    }
    return res;
  }
  void copy_on_write(){
    if (!ptr.unique()){
      ptr.reset(new PolyImpl(*ptr));
    }
  }
  void print() const {
    ptr->print();
  }
  //* resource managed by Singular
  char* c_string() const{

    return p_String(ptr->p,ptr->getRing(),ptr->getRing());
  }

  PolyBase(ring r=currRing):ptr(new PolyImpl((poly) NULL,r)){
  }

  PolyBase(const char* c, ring r=currRing):ptr(new PolyImpl((poly)NULL,r)){
    //p_Read takes no const so do
    char* cp=(char*) omalloc((strlen(c)+1)*sizeof(char));
    strcpy(cp,c);
    p_Read(cp,ptr->p,r);
    omfree(cp);
  }
  PolyBase(const PolyBase&p):ptr(p.ptr){
  }



  PolyBase& operator+=(const PolyBase& p2){
    checkIsSameRing(p2);
    copy_on_write();
    *ptr += *p2.ptr;

    return *this;
  }
  PolyBase& operator*=(const Poly & p2);
  PolyBase& operator*=(Number n){
    copy_on_write();
    *ptr *=n;

    return *this;
  }
  /*  void print(){
     StringSetS("");
     write();
     PrintS(StringAppendS(""));
     }*/
  virtual ~PolyBase(){}
  PolyBase(poly p, ring r):ptr(new PolyImpl(p_Copy(p,r),r)){
  }
  PolyBase(poly p, ring r,int):ptr(new PolyImpl(p,r,0)){
  }
  /*Poly(Poly& p){
    ptr=p.ptr;
    }*/

  PolyInputIterator<create_type> begin(){
    return PolyInputIterator<create_type>(ptr->p,ptr->getRing());
  }
  PolyInputIterator<create_type> end(){
    return PolyInputIterator<create_type>(NULL, ptr->getRing());
  }
  ring getRing() const{
    return ptr->getRing();
  }
  int lmTotalDegree() const{
    return pTotaldegree(ptr->p);
  }
  Number leadCoef(){
    return ptr->leadCoef();
  }
  create_type operator-(){
    create_type erg(*this);
    erg*=Number(-1,ptr->getRing());
    return erg;
  }
 protected:

  PolyBase(PolyImpl& impl):ptr(&impl){

  }
  poly getInternalReference(){
    return ptr->getInternalReference();
  }
 protected:

  shared_ptr<PolyImpl> ptr;

};

class Poly: public PolyBase<POLY_VARIANT_RING, Poly, MyErrorHandler>{
 private:
    typedef PolyBase<POLY_VARIANT_RING, Poly,MyErrorHandler> Base;
  friend class Vector;
  friend class PolyBase<POLY_VARIANT_MODUL,Vector,MyErrorHandler>;
 public:

  Poly(ring r=currRing):Base ((poly)NULL,r,0){
  }
  Poly(int n, ring r=currRing):Base(*(new PolyImpl(n,r))){

  }
  Poly(const char* c, ring r=currRing):Base(c,r){

  }
  Poly(const Base& p):Base(p){
  }

  Poly(const Number& n):Base(*(new PolyImpl(n))){

  }
  Poly(poly p, ring r):Base(p,r){

  }
  Poly(poly p, ring r, int):Base(p,r,0){
  }
  Poly(const std::vector<int>& v, ring r=currRing):Base(*(new PolyImpl((poly) NULL,r))){
    unsigned int i;
    int s=v.size();
    poly p=p_ISet(1,r);
    for(i=0;i<v.size();i++){
      pSetExp(p,i+1,v[i]);
    }
    pSetm(p);
    ptr.reset(new PolyImpl(p,r));
  }
  /*  Poly& operator+=(const Number& n){
  Poly p2(n);
  ((PolyBase<POLY_VARIANT_RING, Poly>&) (*this))+=p2;
  return *this;
  }*/
  Poly& operator+=(const Poly& p ){

    ((Base&)*this)+=p;
    return *this;
  }
  Poly& operator+=(const Base& p ){

    ((Base&)*this)+=p;
    return *this;
  }
  friend inline bool operator==(const Poly& p1, const Poly& p2);

};
class Vector: public PolyBase<POLY_VARIANT_MODUL, Vector, MyErrorHandler>{
 private:
    typedef PolyBase<POLY_VARIANT_MODUL, Vector, MyErrorHandler> Base;
 public:

  Vector(ring r=currRing):Base ((poly)NULL,r,0){
  }
  Vector(int n, ring r=currRing):Base(*(new PolyImpl(n,r))){

  }
  Vector(const char* c, ring r=currRing):Base(c,r){

  }
  Vector(const Base& p):Base(p){
  }


  Vector(poly p, ring r):Base(p,r){

  }
  Vector(poly p, ring r, int):Base(p,r,0){
  }
  Vector(std::vector<int> v, ring r=currRing):Base(*(new PolyImpl((poly) NULL,r))){
    unsigned int i;
    int s=v.size();
    poly p=p_ISet(1,r);
    for(i=0;i<v.size();i++){
      pSetExp(p,i+1,v[i]);
    }
    pSetm(p);
    ptr.reset(new PolyImpl(p,r));
  }
  /*  Poly& operator+=(const Number& n){
  Poly p2(n);
  ((PolyBase<POLY_VARIANT_MODUL, Poly>&) (*this))+=p2;
  return *this;
  }*/
  Vector& operator+=(const Vector& p ){

    ((Base&)*this)+=p;
    return *this;
  }
  Vector& operator+=(const Base& p ){

    ((Base&)*this)+=p;
    return *this;
  }
  friend inline bool operator==(const Vector& p1, const Vector& p2);
};

//typedef Poly PolyBase<POLY_VARIANT_RING>::create_type;
/*template <poly_variant v, class c> inline PolyBase<v> operator+(const PolyBase<v,c>& p1, const PolyBase<v,c>& p2){
    PolyImpl* res=new PolyImpl(*p1.ptr);
    *res+=*p2.ptr;
    return(PolyBase<v,c>(*res));
    }*/
/*template <poly_variant v> inline PolyBase<v> operator*(const PolyBase<v>& p1, const PolyBase<v>& p2){
    PolyImpl* res=new PolyImpl(*p1.ptr);
    *res *= *p2.ptr;
    return(PolyBase<v> (*res));
    }*/
/*template <class c> inline PolyBase<POLY_VARIANT_MODUL> operator*(const PolyBase<POLY_VARIANT_MODUL>& p1, const Number& n){
  PolyBase<POLY_VARIANT_MODUL> erg(p1);
  erg*=n;
  return erg;
  }*/
inline Poly operator*(const Poly& p, const Poly& p2){
  Poly erg=p;
  erg*=p2;
  return erg;
}
inline Vector operator*(const Number& n, const Vector& v){
  Vector res=v;
  res*=n;
  return res;
}

//assumes monomials commute with numbers
template <poly_variant variant, class create_type, class error_traits>
  inline typename PolyBase<variant,create_type, error_traits>::create_type
  operator*
  (const Number& n,
   const PolyBase<variant,create_type, class error_tratis>& p)
{
  typename PolyBase<variant, create_type,error_traits>::create_type erg(p);
  erg*=n;
  return erg;
}

inline Vector operator*(const Poly& p, const Vector& v){
  Vector res(v);
  res*=p;
  return res;
}
inline Poly operator+(const Poly& p1, const Number& n){
 Poly f(p1);
  f+=n;
  return f;
  }
inline bool operator==(const Poly& p1, const Poly& p2){
  ring r1=p1.getRing();
  ring r2=p2.getRing();
  if (r1!=r2) return false;
  return p_EqualPolys(p1.ptr->p,p2.ptr->p,r1);
}
inline bool operator==(const Vector& p1, const Vector& p2){
  ring r1=p1.getRing();
  ring r2=p2.getRing();
  if (r1!=r2) return false;
  return p_EqualPolys(p1.ptr->p,p2.ptr->p,r1);
}
template <poly_variant variant, class create_type,class error_traits>
  inline typename PolyBase<variant,create_type,error_traits>::create_type
  operator+
  (const PolyBase<variant,create_type,error_traits>& b1,
   const PolyBase<variant,create_type,error_traits>& b2)
{
  typename PolyBase<variant, create_type, error_traits>::create_type erg(b1);
  erg+=b2;
  return erg;
}
inline Vector unitVector(int i,ring r=currRing){
  poly p=p_ISet(1,r);
  p_SetComp(p,i,r);
  return Vector(p,r,0);
}
inline Poly operator*(const Number& n, const Poly & p){
  Poly res=p;
  res*=n;
  return res;
}
template <poly_variant variant, class create_type, class error_traits>

inline PolyBase<variant, create_type, error_traits>&
PolyBase<variant, create_type, error_traits>::operator*=(const Poly & p2){
    copy_on_write();
    *ptr *= *p2.ptr;

    return *this;
  }
#endif
