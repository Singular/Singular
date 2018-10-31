/*
 * gfanlib_circuittabletypeint.h
 *
 *  Created on: Apr 10, 2016
 *      Author: anders
 */

#ifndef GFANLIB_CIRCUITTABLEINT_H_
#define GFANLIB_CIRCUITTABLEINT_H_

#include <cstdint>
#include <exception>
#include <sstream>
#include <assert.h>

namespace gfan{

class MVMachineIntegerOverflow: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Exception: Overflow (or possible future overflow) in 32/64 bit integers in tropical homotopy.";
  }
};

extern MVMachineIntegerOverflow MVMachineIntegerOverflow;

/*
 * The philosophy here is that if this class overflows, then the computation needs to be restarted. Therefore
 * all overflows must be caught.
 */
class CircuitTableInt32{
 public:
        class Divisor{
        public:
                int32_t v;
                int shift;
                int32_t multiplicativeInverse;
                Divisor(CircuitTableInt32 const &a)// for exact division
                { // A good place to read about these tricks seems to be the book "Hacker's Delight" by Warren.
                        v=a.v;
                        shift=0;
                        int32_t t=v;
//                        uint32_t t=v;
                        assert(t);
                        while(!(t&1)){t>>=        1;shift++;}
                        uint32_t inverse=t;
                        while(t*inverse!=1)inverse*=2-t*inverse;
                        multiplicativeInverse=inverse;
                }
        };
        class Double{
        public:
                int64_t v;
                Double():v(0){};
                Double(int64_t a):v(a){};
                Double &operator+=(Double a){v+=a.v;return *this;}
                Double &operator-=(Double a){v-=a.v;return *this;}
                CircuitTableInt32 castToSingle()const;
                bool isZero()const{return v==0;}
                bool isNegative()const{return v<0;}
                Double operator-()const{Double ret;ret.v=-v;return ret;}
                friend Double operator-(Double const &a, Double const &b){return Double(a.v-b.v);}
                friend Double operator+(Double const &a, Double const &b){return Double(a.v+b.v);}
                Double &addWithOverflowCheck(Double const &a)
                {
                        if(a.v<0 || v<0 || a.v>1000000000000000000 || v>1000000000000000000) throw MVMachineIntegerOverflow;
                        v+=a.v;
                        return *this;
                }
                std::string toString()const{std::stringstream s;s<<v;return s.str();}
        };
 private:
public:
        int32_t v;
        friend CircuitTableInt32 operator+(CircuitTableInt32 const &a, CircuitTableInt32 const &b){return CircuitTableInt32(a.v+b.v);}
        friend CircuitTableInt32 operator-(CircuitTableInt32 const &a, CircuitTableInt32 const &b){return CircuitTableInt32(a.v-b.v);}
        friend CircuitTableInt32 operator*(CircuitTableInt32 const &a, CircuitTableInt32 const &b){return CircuitTableInt32(a.v*b.v);}
 public:
        // In the code products of CircuitTableInt32 objects are summed. To avoid overflows one of the factors must "fit in half" and the "number of summands" may not exceed a certain number. The bounds are specified in the following functions:
        bool fitsInHalf()const{return v>-30000 && v<30000;}// Is it better to allow only non-negative numbers?
        static bool isSuitableSummationNumber(int numberOfSummands){return numberOfSummands<30000;}
        CircuitTableInt32(){v=0;};
        CircuitTableInt32(CircuitTableInt32 const &m):v(m.v){}
        CircuitTableInt32(int32_t val):v(val){};
        class CircuitTableInt32 &operator=(int32_t a){v=a;return *this;}
        CircuitTableInt32 operator-()const{CircuitTableInt32 ret;ret.v=-v;return ret;}
        CircuitTableInt32 &operator-=(CircuitTableInt32 a){v-=a.v;return *this;}
        CircuitTableInt32 &operator+=(CircuitTableInt32 a){v+=a.v;return *this;}
        CircuitTableInt32 &operator*=(CircuitTableInt32 a){v*=a.v;return *this;}
        friend bool operator<=(CircuitTableInt32 const &a, CircuitTableInt32 const &b){return a.v<=b.v;}
        friend bool operator==(CircuitTableInt32 const &a, CircuitTableInt32 const &b){return a.v==b.v;}
        bool isZero()const{return v==0;}
        bool isOne()const{return v==1;}
        bool isNonZero()const{return v!=0;}
        bool isNegative()const{return v<0;}
        bool isPositive()const{return v>0;}
        friend int determinantSign1(CircuitTableInt32 const &a, CircuitTableInt32 const &c, CircuitTableInt32 const &b, CircuitTableInt32 const &d)//NOTICE MIXED ORDER. MUST WORK BY EXTENDING
        {
                int64_t r=((int64_t)a.v)*((int64_t)c.v)-((int64_t)b.v)*((int64_t)d.v);
                if(r>0)return 1;
                if(r<0)return -1;
                return 0;
        }
        friend int determinantSign2(CircuitTableInt32::Double const &a, CircuitTableInt32 const &c, CircuitTableInt32::Double const &b, CircuitTableInt32 const &d)//NOTICE MIXED ORDER. MUST WORK BY EXTENDING
        {
                int64_t r1=(a.v)*((int64_t)c.v);
                int64_t r2=(b.v)*((int64_t)d.v);
                if(r1>r2)return 1;
                if(r1<r2)return -1;
                return 0;
        }
        std::string toString()const{std::stringstream s;s<<v;return s.str();}
        friend std::ostream &operator<<(std::ostream &f, CircuitTableInt32 const &a){f<<(int)a.v;return f;}
        Double extend()const{Double ret;ret.v=v;return ret;}
        CircuitTableInt32 &maddWithOverflowChecking(CircuitTableInt32 const &a, CircuitTableInt32 const&b){Double t=this->extend();t+=extendedMultiplication(a,b);*this=t.castToSingle();return *this;}
        CircuitTableInt32 &msubWithOverflowChecking(CircuitTableInt32 const &a, CircuitTableInt32 const&b){Double s=this->extend();s-=extendedMultiplication(a,b);*this=s.castToSingle();return *this;}
        CircuitTableInt32 &subWithOverflowChecking(CircuitTableInt32 const &a){Double t=this->extend();t-=a.extend();*this=t.castToSingle();return *this;}
        CircuitTableInt32 &addWithOverflowChecking(CircuitTableInt32 const &a){Double t=this->extend();t+=a.extend();*this=t.castToSingle();return *this;}
        CircuitTableInt32 negatedWithOverflowChecking()const{return (-extend()).castToSingle();}
        friend Double extendedMultiplication(CircuitTableInt32 const &a, CircuitTableInt32 const &b){Double ret;ret.v=((int64_t)a.v)*((int64_t)b.v);return ret;}
        friend Double extendedMultiplication(CircuitTableInt32 const &a, int32_t b){Double ret;ret.v=((int64_t)a.v)*((int64_t)b);return ret;}//to be removed?
        friend CircuitTableInt32 min(CircuitTableInt32 const &a, CircuitTableInt32 const &b){return (a.v>=b.v)?b:a;}
        friend CircuitTableInt32 max(CircuitTableInt32 const &a, CircuitTableInt32 const &b){return (a.v<=b.v)?b:a;}
        friend CircuitTableInt32 negabs(CircuitTableInt32 const &a){return min(a,-a);}
        friend CircuitTableInt32 dotDiv(CircuitTableInt32 const &s, CircuitTableInt32 const &a, CircuitTableInt32 const &t, CircuitTableInt32 const &b, CircuitTableInt32::Divisor const &q)
        {
                return CircuitTableInt32(((((int32_t)(((uint64_t)(((int64_t)s.v)*((int64_t)a.v)+((int64_t)t.v)*((int64_t)b.v)))>>q.shift)))*q.multiplicativeInverse));
        }
        friend void dotDivAssign(CircuitTableInt32 &s, CircuitTableInt32 const &a, CircuitTableInt32 const &t, CircuitTableInt32 const &b, CircuitTableInt32::Divisor const &q)//as above but assigning to first argument. This helps the vectorizer.
        {
                s.v=((((int32_t)(((uint64_t)(((int64_t)s.v)*((int64_t)a.v)+((int64_t)t.v)*((int64_t)b.v)))>>q.shift)))*q.multiplicativeInverse);
        }
        static int MIN(int32_t a, int32_t b)
        {
                return (a<b)?a:b;
        }
        static int MAX(int32_t a, int32_t b)
        {
                return (a>b)?a:b;
        }
        static CircuitTableInt32 computeNegativeBound(CircuitTableInt32 * __restrict__ Ai, int w)
        {
                CircuitTableInt32 M=0;
                CircuitTableInt32 m=0;
                for(int j=0;j<w;j++)
                {
                        m.v=MIN(m.v,Ai[j].v);
                        M.v=MAX(M.v,Ai[j].v);
                }
                return min(m,-M);
        }
        static CircuitTableInt32 quickNoShiftBounded(CircuitTableInt32 * __restrict__ a, CircuitTableInt32 * __restrict__ b, CircuitTableInt32 s, CircuitTableInt32 t, CircuitTableInt32::Divisor denominatorDivisor,int c)
        {
                CircuitTableInt32 *aa=a;
                CircuitTableInt32 *bb=b;
                CircuitTableInt32 max=0;
            CircuitTableInt32 min=0;
            CircuitTableInt32 ret=0;
            for(int i=0;i<c;i++)
              {
                    aa[i].v=((s.v*denominatorDivisor.multiplicativeInverse)*aa[i].v+
                                    (t.v*denominatorDivisor.multiplicativeInverse)*bb[i].v);
                    min.v=MIN(min.v,aa[i].v);
                    max.v=MAX(max.v,aa[i].v);
              }
            if(-max<=min)min=-max;
            ret=min;
            return ret;
        }
        static CircuitTableInt32 dotDivVector(CircuitTableInt32 * __restrict__ aa, CircuitTableInt32 * __restrict__ bb, CircuitTableInt32 s, CircuitTableInt32 t, CircuitTableInt32::Divisor denominatorDivisor,int c, CircuitTableInt32 boundA, CircuitTableInt32 boundB)__attribute__ ((always_inline))//assigning to first argument. The return negative of the return value is an upper bound on the absolute values of the produced entries
        {
                while(((s.v|t.v)&1)==0 && denominatorDivisor.shift>0){s.v>>=1;t.v>>=1;denominatorDivisor.shift--;denominatorDivisor.v>>=1;}

                CircuitTableInt32 max=0;
                CircuitTableInt32 min=0;

          // This is a bound on the absolute value of the sums of products before dividing by the denominator
          // The calculation can overflow, but since we are casting to unsigned long, this is not a problem.
          uint64_t positiveResultBoundTimesD=(negabs(t).v*((int64_t)boundA.v)+negabs(s).v*((int64_t)boundB.v));

          /* The first case is the case where we know that the intermediate results fit in 32bit before shifting down.
           * In other words, the shifted-in bits of the results are equal.
           * In that case the intermediate result can be computed with 32bits.
           */
          if(positiveResultBoundTimesD<((((int64_t)0x40000000)*denominatorDivisor.v)>>denominatorDivisor.shift))//check this carefully
          {//FAST VERSION
                  // debug<<s.v<<" "<<t.v<<" "<<denominatorDivisor.v<<" "<<denominatorDivisor.shift<<"\n";
                  if(denominatorDivisor.shift==0)return quickNoShiftBounded(aa,bb,s,t,denominatorDivisor,c);
                  for(int i=0;i<c;i++)
                  {
                          aa[i].v=((s.v*denominatorDivisor.multiplicativeInverse)*aa[i].v+
                                          (t.v*denominatorDivisor.multiplicativeInverse)*bb[i].v)>>denominatorDivisor.shift;
                          min.v=MIN(min.v,aa[i].v);
                          max.v=MAX(max.v,aa[i].v);
                  }
                  if(-max<=min)min=-max;
                  return min;
          }
          else
          {
                  /* The next case would be to check if the result is known to fit in 32bit.
                   * In that case intermediate results would be handled in 64 bit,
                   * but the final result would not have to be checked for 32 bit overflows.
                   */
                  if(positiveResultBoundTimesD<((((int64_t)0x40000000)*denominatorDivisor.v)))
                  {
                          for(int i=0;i<c;i++)
                          {
                                  aa[i].v=((((int32_t)(((uint64_t)(((int64_t)s.v)*((int64_t)aa[i].v)+((int64_t)t.v)*((int64_t)bb[i].v)))>>denominatorDivisor.shift)))*denominatorDivisor.multiplicativeInverse);
                                  if(max<=aa[i])max=aa[i];
                                  if(aa[i]<=min)min=aa[i];
                          }
                          if(-max<=min)min=-max;
                  }
                  /* The last case would be to where we don't know that the results will fit in 32 bit.
                   * Since we need to compute max and min anyway, we can compute these quantities in 64bit
                   * and just check if they fit in 32bit at the end.
                   */
                  else
                  {
                          bool doesOverflow=(((uint32_t)t.v)==0x80000000);
                          int64_t min64=0;
                          int64_t max64=0;
                          for(int i=0;i<c;i++)
                          {
                                  // In the unlikely event that all the numbers to be multiplied are -2^31, the following code will overflow. Therefore the overflow flag was set as above.
                                  int64_t temp=(((int64_t)s.v)*((int64_t)aa[i].v)+((int64_t)t.v)*((int64_t)bb[i].v))/denominatorDivisor.v;
                                  if(max64<=temp)max64=temp;
                                  if(temp<=min64)min64=temp;
                                  aa[i].v=temp;
                          }
                          if(-max64<=min64)min64=-max64;
                          if(min64<=-0x7fffffff)doesOverflow=true;
                          if(doesOverflow)throw MVMachineIntegerOverflow;
                          min=min64;
                  }
          }
          return min;
        }
};

inline CircuitTableInt32 CircuitTableInt32::Double::castToSingle()const//casts and checks precission
{
        if(v>=0x7fffffff || -v>=0x7fffffff) throw MVMachineIntegerOverflow;
        return CircuitTableInt32(v);
}
}


#endif /* GFANLIB_CIRCUITTABLETYPEINT_H_ */
