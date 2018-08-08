#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "misc/auxiliary.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "reporter/reporter.h"
#include "resources/feResource.h"

#ifndef PLURAL_INTERNAL_DECLARATIONS
#define PLURAL_INTERNAL_DECLARATIONS
#endif

#ifndef TRANSEXT_PRIVATES
#define TRANSEXT_PRIVATES
#endif

#include "polys/nc/gb_hack.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

#include "polys/simpleideals.h"

#include "polys/ext_fields/algext.h"
#include "polys/ext_fields/transext.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define  TS_ASSERT(a) assume(a)
#define  TS_ASSERT_EQUALS(a, b) assume( a == b )
#define  TS_ASSERT_DIFFERS(a, b) assume( a != b )
#define  TS_FAIL(a) WerrorS(a)


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
      CASE(n_algExt);
      CASE(n_transExt);
      CASE(n_long_C);
      CASE(n_Z);
      CASE(n_Zn);
      CASE(n_Znm);
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

    std::stringstream ss;
    {
      char* s = StringEndS();  ss << s; omFree(s);
    }

    return ss.str();

  }

  static inline void PrintSized(/*const*/ number a, const coeffs r, BOOLEAN eoln = TRUE)
  {
    std::clog << _2S(a, r) << ", of size: " << n_Size(a, r);

    if( eoln )
      std::clog << std::endl;
  }



}

using namespace std;




void PrintRing(const ring r)
  {
    rWrite(r); PrintLn();
  #ifdef  RDEBUG
    rDebugPrint(r); PrintLn();
  #endif
  }


  static inline std::string _2S(poly a, const ring r)
  {
    p_Test(a,r);

    StringSetS("");
    p_Write(a, r);

    std::stringstream ss;
    {
      char* s = StringEndS();  ss << s; omFree(s);
    }

    return ss.str();
  }

  static inline void PrintSized(/*const*/ poly a, const ring r, BOOLEAN eoln = TRUE)
  {
    std::clog << _2S(a, r) << ", of size: " << p_Size(a, r);

    if( eoln )
      std::clog << std::endl;
  }

static inline void Delete(poly &p, const ring r)
{
  if( p != NULL )
    p_Delete(&p, r);

  p = NULL;
}

void TestSum(const ring r, const int N)
{
  TS_ASSERT_DIFFERS( r    , NULLp);
  TS_ASSERT_DIFFERS( r->cf, NULLp);


  clog << ( _2S("TEST: sum[0..") + _2S(N) + "]: ");
  clog << endl;

  assume( N > 0 ); // just for now...

  const int ssss = (N * (N+1)) / 2;

  poly sum1 = p_ISet(ssss, r);
  clog<< "poly(N*(N+1)/2) (int: " << ssss << "): "; PrintSized(sum1, r);

  poly s=NULL, ss=NULL, i=NULL, res=NULL;

  s = p_ISet(N  , r);
  i = p_ISet(N+1, r);

  i = p_Mult_q(s, i, r); s = NULL;

  clog<< "poly(N)*poly(N+1): (int: "<< N*(N+1) << "): "; PrintSized(i, r);

  number t = n_Init(2, r->cf);
  clog<< "number(2): "; PrintSized(t, r->cf);

  if( !n_IsZero( t, r->cf) )
  {
    if( i != NULL )
    {
      number ii = p_GetCoeff(i, r);
      clog<< "number(poly(N)*poly(N+1)): "; PrintSized(ii, r->cf);

#ifdef HAVE_RINGS
      TS_ASSERT( n_DivBy(ii, t, r->cf) );
#endif
       res = p_Div_nn(i, t, r); i = NULL;
    }



    clog<< "(poly(N)*poly(N+1))/number(2): "; PrintSized(res, r);
    poly d = p_Sub(p_Copy(res, r), p_Copy(sum1, r), r);

    if( d != NULL )
      TS_ASSERT( n_IsZeroDivisor(p_GetCoeff(d, r), r->cf) );

    Delete(d, r);

    if( n_GetChar(r->cf) == 0 )
    {
      TS_ASSERT( p_EqualPolys(sum1, res, r) );
      TS_ASSERT( p_EqualPolys(res, sum1, r) );
    }
  } else
    TS_ASSERT_EQUALS( n_GetChar(r->cf), 2);

  n_Delete(&t, r->cf);


  s = NULL;
  ss = NULL;
  for( int k = N; k >= 0; k-- )
  {
    i = p_ISet(k, r);
    s = p_Add_q(s, i, r); // s += i

    i = p_Neg( p_ISet(k, r), r );
    ss = p_Add_q(ss, i, r); // ss -= i
  }

  clog<< "ss(-sum): "; PrintSized(ss, r);

  ss = p_Neg(ss, r); // ss = -ss

  clog<< "real sum    : "; PrintSized(s, r);
  clog<< "real sum(--): "; PrintSized(ss, r);

  TS_ASSERT( p_EqualPolys(s, ss, r) );
  TS_ASSERT( p_EqualPolys(ss, s, r) );

//   TODO(somebody, fix the delete method!);

  Delete(sum1, r);
  Delete(res, r);

  Delete(s, r);
  Delete(ss, r);

  clog << ( " >>> TEST DONE!" );
  clog << endl;

}

void Test(const ring r)
{
  if( r == NULL )
      TS_FAIL("Could not get needed ring");
  else
  {
    TestSum( r, 10 );
    TestSum( r, 100 );
    TestSum( r, 101 );
    TestSum( r, 1001 );
    TestSum( r, 9000 );
  }
}

  void test_Z13_t()
  {
    cout << "Creating  Z/13[t]: " << endl;

    char* n[] = {(char*)"t"};
    ring r = rDefault( 13, 1, n);
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( !rField_is_Q(r) );

    TS_ASSERT( rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 11) );
    TS_ASSERT( rField_is_Zp(r, 13) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    Test(r);

    rDelete(r);
  }

  void test_QQ_t()
  {
    cout << "Creating  Q[s]: " << endl;

    char* n[] = {(char*)"s"};
    ring r = rDefault( 0, 1, n);
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( rField_is_Q(r) );

    TS_ASSERT( !rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 11) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    Test(r);

    rDelete(r);
  }

  void test_Z11_x_y_z()
  {
     cout << "Creating  Z/11[x, y, z]: " << endl;

     char* n[] = {(char*)"x", (char*)"y", (char*)"z"};
     ring r = rDefault( 11, 3, n);
     TS_ASSERT_DIFFERS( r, NULLp );

     PrintRing(r);

     TS_ASSERT( rField_is_Domain(r) );
     TS_ASSERT( !rField_is_Q(r) );

     TS_ASSERT( rField_is_Zp(r) );
     TS_ASSERT( rField_is_Zp(r, 11) );
     TS_ASSERT( !rField_is_Zp(r, 13) );

     TS_ASSERT_EQUALS( rVar(r), 3);

     Test(r);

     rDelete(r);
  }
   void test_QQ_x_y_z()
   {
     cout << "Creating  QQ[x, y, z, u]: " << endl;

     char* n[] = {(char*)"x", (char*)"y", (char*)"z", (char*)"u"};
     ring r = rDefault( 0, 4, n);
     TS_ASSERT_DIFFERS( r, NULLp );

     PrintRing(r);

     TS_ASSERT( rField_is_Domain(r) );
     TS_ASSERT( rField_is_Q(r) );

     TS_ASSERT( !rField_is_Zp(r) );
     TS_ASSERT( !rField_is_Zp(r, 11) );

     TS_ASSERT_EQUALS( rVar(r), 4);

     Test(r);

     rDelete(r);
   }


   void test_Z13_t_GF()
   {
     cout << "Creating  GF[t]: " << endl;

     char* n[] = {(char*)"t"};

     GFInfo param;

     param.GFChar= 5;
     param.GFDegree= 2;
     param.GFPar_name= (const char*)"Q";

     const coeffs cf = nInitChar( n_GF, &param );

     if( cf == NULL )
       TS_FAIL("Could not get needed coeff. domain");
     else
     {
       ring r = rDefault( cf, 1, n);  // now cf belongs to r!
       TS_ASSERT_DIFFERS( r, NULLp );

       PrintRing(r);

       TS_ASSERT( rField_is_Domain(r) );
       TS_ASSERT( !rField_is_Q(r) );

       TS_ASSERT( !rField_is_Zp(r) );
       TS_ASSERT( !rField_is_Zp(r, 11) );
       TS_ASSERT( !rField_is_Zp(r, 13) );
       TS_ASSERT( rField_is_GF(r) );

       TS_ASSERT( rField_is_GF(r, 5) );
       TS_ASSERT( !rField_is_GF(r, 25) );

       TS_ASSERT_EQUALS( rVar(r), 1);

       Test(r);

       rDelete(r); // kills 'cf' as well!
     }
   }


int main( int, char *argv[] )
{
  assume( sizeof(long) == SIZEOF_LONG );

  if( sizeof(long) != SIZEOF_LONG )
  {
    WerrorS("Bad config.h: wrong size of long!");

    return(1);
  }


  feInitResources(argv[0]);

  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  PrintLn();

  { char* s = StringEndS(); PrintS(s); omFree(s); }


   test_Z13_t();
   test_QQ_t();
   test_Z11_x_y_z();
   test_QQ_x_y_z();
   test_Z13_t_GF();

   return 0;
}
