//$Id: Poly.h,v 1.1 2005-08-04 15:50:07 bricken Exp $

#include "mod2.h"

#ifndef POLYCPP_HEADER
#define POLYCPP_HEADER
#include "numbers.h"
#include "febase.h"
#include "polys.h"
#include "ring.h"
#include <boost/shared_ptr.hpp>
#define BOOST_DISABLE_THREADS
//PolyImpl is a 08/15 poly wrapper
//Poly wraps around PolyImpl with reference counting using boost
class PolyImpl{
  
 public:
  ring getRing() const{
    return r;
  }
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
  PolyImpl& operator=(const PolyImpl& p2){
    //durch Reihenfolge Selbstzuweisungen berücksichtigt
    if (this==&p2) return *this;
    poly pc=p_Copy(p2.p,p2.r);
    p_Delete(&p,r);
    r=p2.r;
    p=pc;
    return *this;
  }
  PolyImpl operator-(){
    PolyImpl t(*this);
    t.p=p_Copy(p,r);
    t.p=p_Neg(t.p,r);
    return t;
  }
  PolyImpl& operator+=(const PolyImpl & p2){
    if (r!=p2.r){
      Werror("not the same ring");
      return *this;
    }
    if (this==&p2){
      number two=n_Init(2,r);
      p_Mult_nn(p,two,r);
      return *this;
    }
    p=p_Add_q(p,p_Copy(p2.p,p2.r),r);
   
    return *this;
  }
  PolyImpl& operator*=(const PolyImpl & p2){
    if (r!=p2.r){
      Werror("not the same ring");
      return *this;
    }
    if (this==&p2){
      poly pc=p_Copy(p,r);
      p=p_Mult_q(p,p2.p,r);
      return *this;
    }
    p=p_Mult_q(p,p_Copy(p2.p,p2.r),r);
    return *this;
  }
  PolyImpl& operator-=(const PolyImpl & p2){
    if (r!=p2.r){
      Werror("not the same ring");
      return *this;
    }
    if (this==&p2){
      p_Delete(&p,r);
      p=NULL;
      return *this;
    }

    poly pc=p_Copy(p2.p,p2.r);
    pc=p_Neg(pc,r);
    p=p_Add_q(p,pc,r);

    
    return *this;
  }
  //Div not available for rings other than currRing
/*   PolyImpl& operator/=(const PolyImpl & p2){ */
/*     if (r!=p2.r){ */
/*       Werror("not the same ring"); */
/*       return *this; */
/*     } */
/*     if (this==&p2){ */
/*       poly one=p_ISet(1,r); */
/*       p_Delete(&p,r); */
/*       p=one; */
/*       return *this; */
/*     } */
/*     number nv=n_Div(n,p2.n,r); */
/*     n_Delete(&n,r); */
/*     n=nv; */
/*     return *this; */
/*   } */










  PolyImpl& operator=(int n){
 
    p_Delete(&p,r);
    p=p_ISet(n,r);
    return *this;
 
  }
  

  PolyImpl(){
    r=currRing;
    p=NULL;
  }
  PolyImpl(const PolyImpl & p){
    r=p.r;
    this->p=p_Copy(p.p,r);
  }
  PolyImpl(poly p, ring r){
    this->p=p_Copy(p,r);
    this->r=r;
  }
  PolyImpl(int n, ring r){
    this->p=p_ISet(n,r);
    this->r=r;
  }
  explicit PolyImpl(int n){
    r=currRing;
    this->p=p_ISet(n,r);
  }
  void print(){
    p_Write(p,r,r);
  }

  virtual ~PolyImpl(){
    p_Delete(&p,r);
  }

 protected:
  poly p;
  ring r;

};

PolyImpl operator+(const PolyImpl &p1, const PolyImpl& p2){
  PolyImpl erg(p1);
  erg+=p2;
  return erg;
}
PolyImpl operator*(const PolyImpl &p1, const PolyImpl& p2){
  PolyImpl erg(p1);
  erg*=p2;
  return erg;
}
PolyImpl operator-(const PolyImpl &p1, const PolyImpl& p2){
  PolyImpl erg(p1);
  erg-=p2;
  return erg;
}
/*PolyImpl operator/(const PolyImpl &p1, const PolyImpl& p2){
  PolyImpl erg(p1);
  erg/=p2;
  return erg;
  }*/
/*
bool operator==(const PolyImpl &p1, const PolyImpl& p2){
  if(p1.r!=p2.r)
    return false;
  return n_Equal(p1.n,p2.n,p1.r);
  }*/
//Equal Polys not available for oth. rings than currRing


PolyImpl operator+(const PolyImpl &p1, int p2){
  PolyImpl erg(p1);
  erg+=PolyImpl(p2,p1.r);
  return erg;
}
PolyImpl operator*(const PolyImpl &p1, int p2){
  PolyImpl erg(p1);
  erg*=PolyImpl(p2,p1.r);
  return erg;
}
PolyImpl operator-(const PolyImpl &p1, int p2){
  PolyImpl erg(p1);
  erg-=PolyImpl(p2,p1.r);
  return erg;
}
/*PolyImpl operator/(const PolyImpl &p1, int p2){
  PolyImpl erg(p1);
  erg/=PolyImpl(p2,p1.r);
  return erg;
  }*/

/*bool operator==(const PolyImpl &p1, int p2){
  return n_Equal(p1.n,PolyImpl(p2,p1.r).n,p1.r);
  }*/
PolyImpl operator+(int p1, const PolyImpl& p2){
  PolyImpl erg(p2);
  return erg+=PolyImpl(p1,p2.getRing());
}
/*PolyImpl operator-(int p1, const PolyImpl& p2){

  PolyImpl erg(p1,p2.r);
  return erg-=p2;
  }*/
/*PolyImpl operator/(int p1, const PolyImpl& p2){
  PolyImpl erg(p1,p2.r);
  return erg/=p2;
  }*/

PolyImpl operator*(int p1, const PolyImpl& p2){
  PolyImpl erg(p2);
  return erg*=PolyImpl(p1,p2.getRing());
}
/*bool operator==(int p1, const PolyImpl& p2){
  return p2==PolyImpl(p1,p2.r);
  }*/
using namespace boost;

class Poly{
 public:
  void copy_on_write(){
    if (!ptr.unique()){
      ptr.reset(new PolyImpl(*ptr));
    }
  }
  void print(){
    ptr->print();
  }


  Poly(){
  }
  Poly(int n, ring r):ptr(new PolyImpl(n,r)){
    
  }
  Poly& operator+=(Poly p2){
    copy_on_write();
    *ptr+=*p2.ptr;
    
    return *this;
  }
  /*  void print(){
     StringSetS("");
     write();
     Print(StringAppendS(""));
     }*/
  virtual ~Poly(){}

 protected:
  Poly(PolyImpl& impl):ptr(&impl){
   
  }
 private:
  shared_ptr<PolyImpl> ptr;
  friend   inline Poly operator+(Poly p1, Poly p2);
  
};

inline Poly operator+(Poly p1, Poly p2){
    PolyImpl* res=new PolyImpl(*p1.ptr);
    *res+=*p2.ptr;
    return(Poly(*res));
}
#endif
