#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <misc/auxiliary.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>

#include <iostream>


// #pragma GCC diagnostic ignored "-Wwrite-strings"
namespace
{
  static inline std::ostream& operator<< (std::ostream& o, const n_coeffType& type)
  {
#define CASE(A) case A: return o << (" " # A) << " ";
    switch( type )
    {
      CASE(n_unknown);
      CASE(n_Zp);
      CASE(n_Q);
      CASE(n_R);
      CASE(n_GF);
      CASE(n_long_R);
      CASE(n_Zp_a);
      CASE(n_Q_a);
      CASE(n_long_C);
      CASE(n_Z);
      CASE(n_Zn);
      CASE(n_Zpn);
      CASE(n_Z2m);
      CASE(n_CF);
      default: return o << "Unknown type: [" << (const unsigned long) type << "]";  
    }   
#undef CASE
    return o;
  }

  template<typename T>
      static inline std::string _2S(T i)
  {
    std::stringstream ss;
    ss << i;
//    std::string s = ss.str();
    return ss.str();
  }


  static inline std::string _2S(number a, const coeffs r)
  {
    n_Test(a,r);
    StringSetS("");  
    n_Write(a, r);

    const char* s = StringAppendS("");

    std::stringstream ss;  ss << s;

    StringSetS("");  
    return ss.str();

  }

  static inline void PrintSized(/*const*/ number a, const coeffs r, BOOLEAN eoln = TRUE)
  {
    std::clog << _2S(a, r) << ", of size: " << n_Size(a, r);

    if( eoln ) 
      std::clog << std::endl;  
  }



}


#endif /* TESTS_COMMON_H */