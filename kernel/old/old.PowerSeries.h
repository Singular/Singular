#ifndef POWER_SERIES_HEADER
#define POWER_SERIES_HEADER
#include <kernel/mod2.h>
#include <kernel/Poly.h>
template <class traits> class PowerSeriesInputIterator:
public std::
iterator<
  std::input_iterator_tag,
  typename traits::expansion_type,
  int,
  shared_ptr<const typename traits::expansion_type>,
  const typename traits::expansion_type
  > {
 private:
  typedef typename traits::denominator_type denominator_type;
  typedef typename traits::numerator_type numerator_type;
  typedef typename traits::expansion_type expansion_type;
  denominator_type denominator;
  numerator_type numerator;
  denominator_type toPot;
  int state;
  expansion_type data;
  denominator_type lastPot;
 public:
  PowerSeriesInputIterator(numerator_type num_arg,
                           denominator_type den_arg):
    data(den_arg.getRing()),
    lastPot(den_arg.getRing()),
    numerator(num_arg),
    denominator(den_arg){

    ring r=denominator.getRing();

    //not the lead coef    Number c=denominator.leadCoef();
    Number c(1,r);
    typename traits::denominator_type::iterator it=denominator.begin();
    typename traits::denominator_type::iterator end=denominator.end();
    while(it!=end){

      if ((*it).isConstant()){
        //change this type
        c=denominator_type(*it).leadCoef();

        break;
      }

      ++it;



    }
    c=Number(1,r)/c;
    numerator*=c;
    denominator*=c;
    toPot=denominator+denominator_type(-1,r);

    toPot*=Number(-1,r);
    //change this type
    lastPot=denominator_type(1,r);
    data=numerator;
    state=0;

  }
  PowerSeriesInputIterator(){
    state=-1;
  }
  void shorten(){
    typename expansion_type::iterator it=data.begin();
    typename expansion_type::iterator end=data.end();
    ring r=data.getRing();
    expansion_type remove(r);
    while(it!=end){
      if(it->lmTotalDegree()<state){
        remove+=expansion_type(*it);
      }
      it++;
    }
    remove*=Number(-1,r);
    data+=remove;
  }
  expansion_type getValue(){
    typename expansion_type::iterator it=data.begin();
    typename expansion_type::iterator end=data.end();
    ring r=data.getRing();
    expansion_type res(r);
    while(it!=end){
      if(it->lmTotalDegree()==state)
        {
          res+=expansion_type(*it);
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
  const expansion_type operator*(){
    return expansion_type(getValue());
  }
  shared_ptr<const expansion_type> operator->(){
    return shared_ptr<const expansion_type>(new expansion_type(getValue()));
  }
  };


template<class traits> class PowerSeriesBase{
 public:
  typedef typename traits::denominator_type denominator_type;
  typedef typename traits::numerator_type numerator_type;
 protected:
  denominator_type denominator;
  numerator_type numerator;
 public:

  PowerSeriesBase(){
  }
  PowerSeriesBase(const numerator_type &a, const denominator_type & b):numerator(a),denominator(b){
    assume(a.getRing()==b.getRing());
    //assume b!=NULL
  }
  typedef PowerSeriesInputIterator<traits> iterator;
  iterator begin(){
    return iterator(numerator,denominator);
  }
  iterator end(){
    return iterator();
  }


};
class PowerSeriesPolyTraits;
class PowerSeriesVectorTraits;
typedef PowerSeriesBase<PowerSeriesPolyTraits> PowerSeries;
typedef PowerSeriesBase<PowerSeriesVectorTraits> VectorPowerSeries;
class PowerSeriesPolyTraits{
 public:
  typedef Poly numerator_type;
  typedef Poly denominator_type;
  typedef PowerSeries create_type;
  typedef Poly expansion_type;
};
class PowerSeriesVectorTraits{
 public:
  typedef Vector numerator_type;
  typedef Poly denominator_type;
  typedef VectorPowerSeries create_type;
  typedef Vector expansion_type;
};


#endif
