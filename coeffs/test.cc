#include "config.h"

#include <auxiliary.h>

#include <coeffs.h>
#include <numbers.h>
#include <reporter.h>
#include <omalloc.h>

#include <longrat.h>
#include <gnumpfl.h>
#include <gnumpc.h>
#include <shortfl.h>
#include <ffields.h>
#include <modulop.h>
#include <rmodulon.h>
#include <rmodulo2m.h>
#include <rintegers.h>

#include <iostream>

using namespace std;


#pragma GCC diagnostic ignored "-Wwrite-strings"

bool Test(const coeffs r)
{
  number a = n_Init(66666, r);
   
  StringSetS("a: ");
  n_Test(a,r);
  n_Write(a, r);
  PrintS(StringAppend("\n"));

  number two = n_Init(2, r);
  
  StringSetS("two: ");
  n_Test(two,r);
  n_Write(two, r);
  PrintS(StringAppend("\n"));

  if (getCoeffType(r) == n_GF) //some special test for GF
  {
    number z = nfPar (0, r); // also any integer instead of 0
    StringSetS("Generator: ");
    n_Test(z,r); n_Write (z,r);
    PrintS(StringAppend("\n"));
    n_Delete(&z, r);    
  }
  
  number aa = n_Add(a, a, r);

  StringSetS("aa = a + a: ");
  n_Test(aa,r); n_Write(aa, r);
  PrintS(StringAppend("\n"));
  
  number aa2 = n_Mult(a, two, r);

  StringSetS("aa2 = a * 2: ");
  n_Test(aa2, r); n_Write(aa2, r);
  PrintS(StringAppend("\n"));

  number aa1 = n_Mult(two, a, r);
 
  StringSetS("aa1 = 2 * a: ");
  n_Test(aa1,r); n_Write(aa1, r);
  PrintS(StringAppend("\n"));


  n_Delete(&a, r);
  n_Delete(&two, r);


  a = n_Sub( aa, aa1, r );
  
  StringSetS("a = aa - aa1: ");
  n_Test(a,r); n_Write(a, r);
  PrintS(StringAppend("\n"));

  if( !n_IsZero(a, r) )
    WarnS("ERROR: a != 0 !!!\n");

  n_Delete(&a, r);



  a = n_Sub( aa, aa2, r );

  StringSetS("a = aa - aa2: ");
  n_Test(a,r); n_Write(a, r);
  PrintS(StringAppend("\n"));

  if( !n_IsZero(a, r) )
    WarnS("ERROR: a != 0 !!!\n");

  n_Delete(&a, r);


  a = n_Sub( aa1, aa2, r );

  StringSetS("a = aa1 - aa2: ");
  n_Test(a,r); n_Write(a, r);
  PrintS(StringAppend("\n"));

  if( !n_IsZero(a, r) )
    WarnS("ERROR: a != 0 !!!\n");

  n_Delete(&a, r);


  
  if( !n_Equal(aa, aa1, r) )
    WarnS("ERROR: aa != aa1  !!!\n");

  if( !n_Equal(aa, aa2, r) )
    WarnS("ERROR: aa != aa2  !!!\n");

  if( !n_Equal(aa1, aa2, r) )
    WarnS("ERROR: aa1 != aa2  !!!\n");
  

  

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
}
  

bool Test(const n_coeffType type, void* p = NULL)
{
  cout  << endl << "----------------------- Testing coeffs: [" << type <<
                "]: -----------------------" << endl;

  const coeffs r = nInitChar( type, p );

  assume( r != NULL );

  nSetChar( r );
  assume( getCoeffType(r) == type );

  assume( r->cfInit != NULL );
  assume( r->cfWrite != NULL );
  assume( r->cfAdd != NULL );
  assume( r->cfDelete != NULL );

  if( type == n_Q )
  {
    assume( r->cfInit == nlInit );
    assume( r->cfWrite == nlWrite );
    assume( r->cfAdd == nlAdd );
    assume( r->cfDelete == nlDelete );    
  }
  else if( type == n_long_R )
  {
    assume( r->cfInit == ngfInit );
    assume( r->cfWrite == ngfWrite );
    assume( r->cfAdd == ngfAdd );
    assume( r->cfDelete == ngfDelete );
  }
  else if( type == n_long_C )
  {
    assume( r->cfInit == ngcInit );
    assume( r->cfWrite == ngcWrite );
    assume( r->cfAdd == ngcAdd );
    assume( r->cfDelete == ngcDelete );    
  }
  else if( type == n_R )
  {
    assume( r->cfInit == nrInit );
    assume( r->cfWrite == nrWrite );
    assume( r->cfAdd == nrAdd );
//    assume( r->cfDelete == nrDelete ); // No?
  }
#ifdef HAVE_RINGS
  else if( type == n_Z2m )
  {
    assume( r->cfInit == nr2mInit );
    assume( r->cfWrite == nr2mWrite );
    assume( r->cfAdd == nr2mAdd );
    assume( r->cfDelete == ndDelete );
  }
  else if( type == n_Zn )
  {
    assume( r->cfInit == nrnInit );
    assume( r->cfWrite == nrnWrite );
    assume( r->cfAdd == nrnAdd );
    assume( r->cfDelete == nrnDelete );
  }
#endif
  else if( type == n_GF )
  {
    assume( r->cfInit == nfInit );
    assume( r->cfWrite == nfWrite );
    assume( r->cfAdd == nfAdd );
    //assume( r->cfDelete == nfDelete );
  }
  else
  {
    // ...
  }

  bool ret = Test( r );

  nKillChar( r );

  return ret;
}




int main()
{
  int c = 0;
  
  n_coeffType type;

  // rings needed for: n_Zp_a, n_Q_a ?

  type = nRegister( n_Zp, npInitChar); assume( type == n_Zp );
  if( Test(type, (void*) 11) )
    c ++;

  
  type = nRegister( n_Q, nlInitChar); assume( type == n_Q );
  if( Test(type) )
    c ++;

  type = nRegister( n_R, nrInitChar); assume( type == n_R );
  if( Test(type) )
    c ++;


#ifdef HAVE_RINGS
  type = nRegister( n_Z, nrzInitChar); assume( type == n_Z );
  if( Test(type) )
    c ++;

  type = nRegister( n_Z2m, nr2mInitChar); assume( type == n_Z2m );
  if( Test(type, (void*) 2) )
    c ++;
#endif


   type = nRegister( n_GF, nfInitChar); assume( type == n_GF );

   GFInfo param;

   param.GFChar= 5;
   param.GFSize= 25;

   param.GFPar_name= (const char*)"q";

   if( Test(type, (void*) &param) )
     c ++;
  
 
  TODO(Somebody, floating arithmetics via GMP rely on two global variables (see setGMPFloatDigits). Fix it!);
  
  setGMPFloatDigits( 10, 5 ); // Init global variables in mpr_complex.cc for gmp_float's...
      
  type = nRegister( n_long_C, ngcInitChar); assume( type == n_long_C );
  if( Test(type) )
    c ++;

  type = nRegister( n_long_R, ngfInitChar); assume( type == n_long_R );
  if( Test(type) )
    c ++;
  
  
#ifdef HAVE_RINGS
  type = nRegister( n_Zn, nrnInitChar); assume( type == n_Zn );

  TODO(Frank, this seems to be a trivial BUG: the parameter 'm' is ignored now in nrnSetExp. Please fix someday!!!)
  if( Test(type, (void*) 3) )
    c ++;
#endif

  
  
  return c;

}
