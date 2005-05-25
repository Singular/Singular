//$Id: wrapper.h,v 1.4 2005-05-25 13:32:00 bricken Exp $
#include "mod2.h"
#include "numbers.h"
#include "febase.h"
#include <Python.h>
#include <boost/python.hpp>

using namespace boost::python;
class Number{
  
public:
    friend Number operator+(const Number& n1, const Number& n2);
    friend Number operator-(const Number& n1, const Number& n2);
    friend Number operator/(const Number& n1, const Number& n2);
    friend Number operator*(const Number& n1, const Number& n2);
    friend bool operator==(const Number& n1, const Number& n2);
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
      t.n=n_Copy(n,r);
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
  Number(int n){
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
static boost::python::object as_str(Number n)
{
  using boost::python::str;
  StringSetS("");
  n.write();
  char* out=StringAppendS("");
  return boost::python::str(out,strlen(out));
}
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
BOOST_PYTHON_MODULE(Singular){
  boost::python::class_<Number>("number")
    .def(boost::python::init <int>())
    .def("__str__", as_str)
    .def(-self)
    .def(self==self)
    .def(self+self)
    .def(self*self)
    .def(self/self)
    .def(self-self);
}
