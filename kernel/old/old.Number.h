#ifndef NUMBERCPP_HEADER
#define NUMBERCPP_HEADER
#include <boost/intrusive_ptr.hpp>
#include <kernel/mod2.h>
#include <coeffs/numbers.h>
#include <polys/monomials/ring.h>
using namespace boost;
inline void intrusive_ptr_add_ref(ring r){
    r->ref++;
    //Print("ref count after add: %d", r->ref);
}
inline void intrusive_ptr_release(ring r){
    if (r->ref<=0) rDelete(r);
    else {
    r->ref--;

    }
    //Print("ref count after release: %d", r->ref);
}

enum poly_variant{
  POLY_VARIANT_RING,
  POLY_VARIANT_MODUL
};
template<poly_variant,class,class> class PolyBase;
class Poly;
//class TrivialErrorHandler;
class Vector;
class Number{

 public:
  friend Number operator+(const Number& n1, const Number& n2);
  friend Number operator-(const Number& n1, const Number& n2);
  friend Number operator/(const Number& n1, const Number& n2);
  friend Number operator*(const Number& n1, const Number& n2);
  friend bool operator==(const Number& n1, const Number& n2);
  friend Number operator+(const Number& n1, int n2);
  friend Number operator-(const Number& n1, int n2);
  friend Number operator/(const Number& n1, int n2);
  friend Number operator*(const Number& n1, int n2);
  friend bool operator==(const Number& n1, int n2);
  friend Number operator+(int n1, const Number& n2);
  friend Number operator-(int n1, const Number& n2);
  friend Number operator/(int n1, const Number& n2);
  friend Number operator*(int n1, const Number& n2);
  friend bool operator==(int n1, const Number& n2);
  friend class Poly;
  friend class Vector;
  //friend class PolyBase<POLY_VARIANT_RING,Poly,TrivialErrorHandler>;
//   friend class PolyBase <poly_variant variant,
//   class create_type_input,
//   class error_handle_traits>;
   template <poly_variant,class,class> friend class PolyBase;
  friend class PolyImpl;
  number as_number() const{
    return n_Copy(n,r.get());
  }
  Number& operator=(const Number& n2){
    //durch Reihenfolge Selbstzuweisungen beruecksichtigt
    number nc=n_Copy(n2.n,n2.r.get());
    n_Delete(&n,r.get());
    r=n2.r;
    n=nc;
    return *this;
  }
  Number operator-(){
    Number t(*this);
    //t.n=n_Copy(n,r);
    t.n=n_InpNeg(t.n,r.get());
    return t;
  }
  Number& operator+=(const Number & n2){
    if (r!=n2.r){
      WerrorS("not the same ring");
      return *this;
    }
    number nv=n_Add(n,n2.n,r.get());
    n_Delete(&n,r.get());
    n=nv;
    return *this;
  }
  Number& operator*=(const Number & n2){
    if (r!=n2.r){
      WerrorS("not the same ring");
      return *this;
    }
    number nv=n_Mult(n,n2.n,r.get());
    n_Delete(&n,r.get());
    n=nv;
    return *this;
  }
  Number& operator-=(const Number & n2){
    if (r!=n2.r){
      WerrorS("not the same ring");
      return *this;
    }
    number nv=n_Sub(n,n2.n,r.get());
    n_Delete(&n,r.get());
    n=nv;
    return *this;
  }
  Number& operator/=(const Number & n2){
    if (r!=n2.r){
      WerrorS("not the same ring");
      return *this;
    }
    number nv=n_Div(n,n2.n,r.get());
    n_Delete(&n,r.get());
    n=nv;
    return *this;
  }

  Number& operator=(int n2){
    n_Delete(&n,r.get());
    n=n_Init(n2,r.get());
    return *this;
  }

  Number& operator+=(int n2){
    number n2n=n_Init(n2,r.get());
    number nv=n_Add(n,n2n,r.get());
    n_Delete(&n,r.get());
    n_Delete(&n2n,r.get());
    n=nv;
    return *this;
  }
  Number& operator*=(int n2){
    number n2n=n_Init(n2,r.get());
    number nv=n_Mult(n,n2n,r.get());
    n_Delete(&n,r.get());
    n_Delete(&n2n,r.get());
    n=nv;
    return *this;
  }
  Number& operator-=(int n2){

    number n2n=n_Init(n2,r);
    number nv=n_Sub(n,n2n,r);
    n_Delete(&n,r.get());
    n_Delete(&n2n,r.get());
    n=nv;
    return *this;
  }
  Number& operator/=(int n2){
    number n2n=n_Init(n2,r.get());
    number nv=n_Div(n,n2n,r.get());
    n_Delete(&n,r.get());
    n_Delete(&n2n,r.get());
    n=nv;
    return *this;
  }



  Number(){
    r=currRing;
    if (r.get()!=NULL)
      n=n_Init(0,r.get());
    else
      n=(number) NULL;
  }
  Number(const Number & n){
    r=n.r;
    this->n=n_Copy(n.n,r.get());
  }
  Number(number n, ring r){
    this->n=n_Copy(n,r);
    this->r=r;
  }
  Number(int n, ring r){

    this->r=r;
    this->n=n_Init(n,r);
  }
  Number(int n, intrusive_ptr<ip_sring> r){
    this->r=r;
    this->n=n_Init(n,r.get());

  }
  explicit Number(int n){
    r=currRing;
    this->n=n_Init(n,r.get());
  }
  void write() const{
    number towrite=n;
    n_Write(towrite,r.get());
  }

  ~Number(){
    if (r!=NULL)
      n_Delete(&n,r.get());
  }
 ring getRing() const{
    return r.get();
 }
 protected:
  number n;
  intrusive_ptr<ip_sring> r;

};

inline Number operator+(const Number &n1, const Number& n2){
  Number erg(n1);
  erg+=n2;
  return erg;
}
inline Number operator*(const Number &n1, const Number& n2){
  Number erg(n1);
  erg*=n2;
  return erg;
}
inline Number operator-(const Number &n1, const Number& n2){
  Number erg(n1);
  erg-=n2;
  return erg;
}
inline Number operator/(const Number &n1, const Number& n2){
  Number erg(n1);
  erg/=n2;
  return erg;
}
inline bool operator==(const Number &n1, const Number& n2){
  if(n1.r!=n2.r)
    return false;
  return n_Equal(n1.n,n2.n,n1.r);
}


inline Number operator+(const Number &n1, int n2){
  Number erg(n1);
  erg+=Number(n2,n1.r);
  return erg;
}
inline Number operator*(const Number &n1, int n2){
  Number erg(n1);
  erg*=Number(n2,n1.r);
  return erg;
}
inline Number operator-(const Number &n1, int n2){
  Number erg(n1);
  erg-=Number(n2,n1.r);
  return erg;
}
inline Number operator/(const Number &n1, int n2){
  Number erg(n1);
  erg/=Number(n2,n1.r);
  return erg;
}
inline bool operator==(const Number &n1, int n2){
  return n_Equal(n1.n,Number(n2,n1.r).n,n1.r);
}
inline Number operator+(int n1, const Number& n2){
  Number erg(n2);
  return erg+=Number(n1,n2.r);
}
inline Number operator-(int n1, const Number& n2){

  Number erg(n1,n2.r);
  return erg-=n2;
}
inline Number operator/(int n1, const Number& n2){
  Number erg(n1,n2.r);
  return erg/=n2;
}

inline Number operator*(int n1, const Number& n2){
  Number erg(n2);
  return erg*=Number(n1,n2.r);
}
inline bool operator==(int n1, const Number& n2){
  return n2==Number(n1,n2.r);
}
#endif
