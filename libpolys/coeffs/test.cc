#include "misc/auxiliary.h"

#include "factory/factory.h"

#include "reporter/reporter.h"
#include "resources/feResource.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "coeffs/longrat.h"
#include "coeffs/gnumpfl.h"
#include "coeffs/gnumpc.h"
#include "coeffs/shortfl.h"
#include "coeffs/ffields.h"
#include "coeffs/modulop.h"
#include "coeffs/rmodulon.h"
#include "coeffs/rmodulo2m.h"
#include "coeffs/rintegers.h"


#include <iostream>

using namespace std;

#pragma GCC diagnostic ignored "-Wwrite-strings"

void Print(/*const*/ number a, const coeffs r, BOOLEAN eoln = TRUE)
{
  n_Test(a,r);

  StringSetS("");
  n_Write(a, r);


  if( eoln )
    PrintLn();

  { char* s = StringEndS(); PrintS(s); omFree(s); }
}


void PrintSized(/*const*/ number a, const coeffs r, BOOLEAN eoln = TRUE)
{
  Print(a, r, FALSE);
  Print(", of size: %d", n_Size(a, r));

  if( eoln )
    PrintLn();
}



bool TestArith(const coeffs r)
{
  number a = n_Init(66666, r);

  PrintS("a: "); PrintSized(a, r);

  number two = n_Init(2, r);

  PrintS("two: "); PrintSized(two, r);

  if (n_NumberOfParameters(r) > 0)
  {
    number z = n_Param(1, r); // also any integer instead of 0//?

    PrintS("Parameter: "); PrintSized(z, r);

    n_Delete(&z, r);
  }

  number aa = n_Add(a, a, r);

  PrintS("aa = a + a: "); PrintSized(aa, r);

  number aa2 = n_Mult(a, two, r);

  PrintS("aa2 = a * 2: "); PrintSized(aa2, r);

  number aa1 = n_Mult(two, a, r);

  PrintS("aa1 = 2 * a: "); PrintSized(aa1, r);

  n_Delete(&a, r);
  n_Delete(&two, r);


  a = n_Sub( aa, aa1, r );

  PrintS("a = aa - aa1: "); PrintSized(a, r);

  if( !n_IsZero(a, r) )
    WarnS("TestArith: ERROR: a != 0 !!!\n");

  n_Delete(&a, r);

  a = n_Sub( aa, aa2, r );

  PrintS("a = aa - aa2: "); PrintSized(a, r);

  if( !n_IsZero(a, r) )
    WarnS("TestArith: ERROR: a != 0 !!!\n");

  n_Delete(&a, r);


  a = n_Sub( aa1, aa2, r );

  PrintS("a = aa1 - aa2: "); PrintSized(a, r);

  if( !n_IsZero(a, r) )
    WarnS("TestArith: ERROR: a != 0 !!!\n");

  n_Delete(&a, r);



  if( !n_Equal(aa, aa1, r) )
    WarnS("TestArith: ERROR: aa != aa1  !!!\n");

  if( !n_Equal(aa, aa2, r) )
    WarnS("TestArith: ERROR: aa != aa2  !!!\n");

  if( !n_Equal(aa1, aa2, r) )
    WarnS("TestArith: ERROR: aa1 != aa2  !!!\n");




  n_Delete(&aa, r);
  n_Delete(&aa1, r);
  n_Delete(&aa2, r);

  return false;
}



namespace
{
  static inline ostream& operator<< (ostream& o, const n_coeffType& type)
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
      default: return o << "Unknown type: [" << (unsigned long) type << "]";
    }
#undef CASE
    return o;
  }
}


bool Test(const n_coeffType type, void* p = NULL)
{
  cout  << endl << "----------------------- Testing coeffs: [" << type << ", " << p <<
                "]: -----------------------" << endl;

  const coeffs r = nInitChar( type, p );

  if( r == NULL ) { cout << "Test: could not get the specified coeff. domain for type: " << type << " and the parameter: " << p << endl; return false; };

  assume( r != NULL );
  nSetChar( r );
  assume( getCoeffType(r) == type );

  assume( r->cfInit != NULL );
  assume( r->cfWriteLong != NULL );
  assume( r->cfAdd != NULL );
  assume( r->cfDelete != NULL );
  assume( r->cfKillChar != NULL );

  bool ret = TestArith( r );

  nKillChar( r );

  return ret;
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

  StringSetS("resources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  PrintLn();

  { char* s = StringEndS(); PrintS(s); omFree(s); }

  int c = 0;

  n_coeffType type;


//  TODO(Frank, Segmentation fault! (if used without omalloc???). Please_ investigate!);
  type =  n_Z2m;
  if( Test(type, (void*) 4) )
    c ++;

  type =  n_Zp;
  if( Test(type, (void*) 101) )
    c ++;

//  TODO(Frank, memory corruption_ if used without omalloc??? Please_ investigate!);

  type = n_Z2m;
  if( Test(type, (void*) 8) )
    c ++;

  type =  n_Q;
  if( Test(type) )
    c ++;

  type = n_R;
  if( Test(type) )
    c ++;

  type = n_Z;
  if( Test(type) )
    c ++;

   type = n_GF;


   GFInfo* param = new GFInfo();

   param->GFChar= 5;
   param->GFDegree= 12;
   param->GFPar_name= (const char*)"q";

   if( Test(type, (void*) param) )
     c ++;

   // it should not be used by numbers... right?
   // TODO: what is our policy wrt param-pointer-ownership?
   delete param;
   // Q: no way to deRegister a type?

   param = new GFInfo();

   param->GFChar= 5;
   param->GFDegree= 2;
   param->GFPar_name= (const char*)"Q";

   if( Test(type, (void*) param) )
     c ++;

   delete param;

  type = n_Zn;

  ZnmInfo Znmparam;
  Znmparam.base= (mpz_ptr) omAlloc (sizeof (mpz_t));
  mpz_init_set_ui (Znmparam.base, 3);
  Znmparam.exp= 1;

  if( Test(type, (void*) &Znmparam) )
    c ++;

  type = n_long_C;
  if( Test(type) )
    c ++;

  type = n_long_R;
  if( Test(type) )
    c ++;

  type = n_Z2m;
  if( Test(type, (void*) 2) )
    c ++;

  // polynomial rings needed for: n_algExt, n_transExt !

  return c;

}
