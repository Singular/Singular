#ifndef NUMBERCPP_HEADER
#define NUMBERCPP_HEADER
#include "mod2.h"
#include "numbers.h"
#include "febase.h"


enum poly_variant{
  POLY_VARIANT_RING,
  POLY_VARIANT_MODUL
};
template<poly_variant,class> class PolyBase;
class Poly;
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
  friend class PolyBase<POLY_VARIANT_RING,Poly>;
 
  friend class PolyImpl;
  Number& operator=(const Number& n2){
    //durch Reihenfolge Selbstzuweisungen berücksichtigt
    number nc=n_Copy(n2.n,n2.r);
    n_Delete(&n,r);
    r=n2.r;
    n=nc;
    return *this;
  }
  Number operator-(){
    Number t(*this);
    //t.n=n_Copy(n,r);
    t.n=n_Neg(t.n,r);
    return t;
  }
  Number& operator+=(const Number & n2){
    if (r!=n2.r){
      Werror("not the same ring");
      return *this;
    }
    number nv=n_Add(n,n2.n,r);
    n_Delete(&n,r);
    n=nv;
    return *this;
  }
  Number& operator*=(const Number & n2){
    if (r!=n2.r){
      Werror("not the same ring");
      return *this;
    }
    number nv=n_Mult(n,n2.n,r);
    n_Delete(&n,r);
    n=nv;
    return *this;
  }
  Number& operator-=(const Number & n2){
    if (r!=n2.r){
      Werror("not the same ring");
      return *this;
    }
    number nv=n_Sub(n,n2.n,r);
    n_Delete(&n,r);
    n=nv;
    return *this;
  }
  Number& operator/=(const Number & n2){
    if (r!=n2.r){
      Werror("not the same ring");
      return *this;
    }
    number nv=n_Div(n,n2.n,r);
    n_Delete(&n,r);
    n=nv;
    return *this;
  }










  Number& operator=(int n2){
    n_Delete(&n,r);
    n=n_Init(n2,r);
    return *this;
  }
  
  Number& operator+=(int n2){
    number n2n=n_Init(n2,r);
    number nv=n_Add(n,n2n,r);
    n_Delete(&n,r);
    n_Delete(&n2n,r);
    n=nv;
    return *this;
  }
  Number& operator*=(int n2){
    number n2n=n_Init(n2,r);
    number nv=n_Mult(n,n2n,r);
    n_Delete(&n,r);
    n_Delete(&n2n,r);
    n=nv;
    return *this;
  }
  Number& operator-=(int n2){

    number n2n=n_Init(n2,r);
    number nv=n_Sub(n,n2n,r);
    n_Delete(&n,r);
    n_Delete(&n2n,r);
    n=nv;
    return *this;
  }
  Number& operator/=(int n2){  
    number n2n=n_Init(n2,r);
    number nv=n_Div(n,n2n,r);
    n_Delete(&n,r);
    n_Delete(&n2n,r);
    n=nv;
    return *this;
  }



  Number(){
    r=currRing;
    n=n_Init(0,r);
  }
  Number(const Number & n){
    r=n.r;
    this->n=n_Copy(n.n,r);
  }
  Number(number n, ring r){
    this->n=n_Copy(n,r);
    this->r=r;
  }
  Number(int n, ring r){
    this->n=n_Init(n,r);
    this->r=r;
  }
  explicit Number(int n){
    r=currRing;
    this->n=n_Init(n,r);
  }
  void write(){
    n_Write(n,r);
  }
  virtual ~Number(){
    n_Delete(&n,r);
  }

 protected:
  number n;
  ring r;

};

Number operator+(const Number &n1, const Number& n2){
  Number erg(n1);
  erg+=n2;
  return erg;
}
Number operator*(const Number &n1, const Number& n2){
  Number erg(n1);
  erg*=n2;
  return erg;
}
Number operator-(const Number &n1, const Number& n2){
  Number erg(n1);
  erg-=n2;
  return erg;
}
Number operator/(const Number &n1, const Number& n2){
  Number erg(n1);
  erg/=n2;
  return erg;
}
bool operator==(const Number &n1, const Number& n2){
  if(n1.r!=n2.r)
    return false;
  return n_Equal(n1.n,n2.n,n1.r);
}


Number operator+(const Number &n1, int n2){
  Number erg(n1);
  erg+=Number(n2,n1.r);
  return erg;
}
Number operator*(const Number &n1, int n2){
  Number erg(n1);
  erg*=Number(n2,n1.r);
  return erg;
}
Number operator-(const Number &n1, int n2){
  Number erg(n1);
  erg-=Number(n2,n1.r);
  return erg;
}
Number operator/(const Number &n1, int n2){
  Number erg(n1);
  erg/=Number(n2,n1.r);
  return erg;
}
bool operator==(const Number &n1, int n2){
  return n_Equal(n1.n,Number(n2,n1.r).n,n1.r);
}
Number operator+(int n1, const Number& n2){
  Number erg(n2);
  return erg+=Number(n1,n2.r);
}
Number operator-(int n1, const Number& n2){

  Number erg(n1,n2.r);
  return erg-=n2;
}
Number operator/(int n1, const Number& n2){
  Number erg(n1,n2.r);
  return erg/=n2;
}

Number operator*(int n1, const Number& n2){
  Number erg(n2);
  return erg*=Number(n1,n2.r);
}
bool operator==(int n1, const Number& n2){
  return n2==Number(n1,n2.r);
}
#endif
