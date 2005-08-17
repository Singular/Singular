#ifndef POWER_SERIES_HEADER
#define POWER_SERIES_HEADER
#include "mod2.h"
#include "Poly.h"
class PowerSeriesInputIterator:public std::iterator<std::input_iterator_tag,Poly,int, shared_ptr<const Poly>,const Poly > {
 private:
  Poly denominator;
  Poly numerator;
  Poly toPot;
  int state;
  Poly data;
  Poly lastPot;
 public:
  PowerSeriesInputIterator(Poly num_arg, Poly den_arg):data(den_arg.getRing()), lastPot(den_arg.getRing()){
    denominator=den_arg;
    ring r=denominator.getRing();
    numerator=num_arg;
    //not the lead coef    Number c=denominator.leadCoef();
    Number c(1,r);
    Poly::iterator it=denominator.begin();
    Poly::iterator end=denominator.end();
    while(it!=end){
     
      if ((*it).isConstant()){
	c=Poly(*it).leadCoef();
	
	break;
      }
      
      ++it;
      
      
 
    }
    c=Number(1,r)/c;
    numerator*=c;
    denominator*=c;
    toPot=denominator+Poly(-1,r);
    
    toPot*=Number(-1,r);
    lastPot=Poly(1,r);
    data=numerator;
    state=0;
    
  }
  PowerSeriesInputIterator(){
    state=-1;
  }
  void shorten(){
    Poly::iterator it=data.begin();
    Poly::iterator end=data.end();
    ring r=data.getRing();
    Poly remove(r);
    while(it!=end){
      if(it->lmTotalDegree()<state){
	remove+=Poly(*it);
      }
      it++;
    }
    remove*=Number(-1,r);
    data+=remove;
  }
  Poly getValue(){
    Poly::iterator it=data.begin();
    Poly::iterator end=data.end();
    ring r=data.getRing();
    Poly res(r);
    while(it!=end){
      if(it->lmTotalDegree()==state)
	{
	  res+=Poly(*it);
	}
      it++;
    }
    return res;
  }
  PowerSeriesInputIterator& operator++(){
    state++;
    shorten();
    lastPot*=toPot;
    
    data+=lastPot*numerator;
    
 
    return *this;
    
  }
  //bad if this are iterators for different PowerSeries
  bool operator==(const PowerSeriesInputIterator& t2){
    return state==t2.state;
  }
  bool operator!=(const PowerSeriesInputIterator& t2){
    return state!=t2.state;
  }



  PowerSeriesInputIterator operator++(int){
    PowerSeriesInputIterator it(*this);
    ++(*this);
    return it;
  }
  const Poly operator*(){
    return Poly(getValue());
  }
  shared_ptr<const Poly> operator->(){
    return shared_ptr<const Poly>(new Poly(getValue()));
  }
  };
class PowerSeries{
 protected:
  Poly denominator;
  Poly numerator;
 public:
  PowerSeries(){
  }
  PowerSeries(const Poly &a, const Poly & b):numerator(a),denominator(b){
    assume(a.getRing()==b.getRing());
    //asume b!=NULL
  }
  typedef PowerSeriesInputIterator iterator;
  iterator begin(){
    return iterator(numerator,denominator);
  }
  iterator end(){
    return iterator();
  }
  

};


#endif
