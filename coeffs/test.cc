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


#include <rmodulon.h>
#include <rmodulo2m.h>

#include <iostream>
using namespace std;


bool Test(const coeffs r)
{
  number a = n_Init(666, r); 
  n_Test(a,r);
  
  number b = n_Add(a, a, r);
  n_Test(b,r);

  StringSetS("a: "); n_Write(a, r);PrintS(StringAppend("\n"));
  StringSetS("b: "); n_Write(b, r);PrintS(StringAppend("\n"));

  n_Delete(&a, r);
  n_Delete(&b, r);

  return false;
}



bool Test(const n_coeffType type)
{

  cout  << endl << "----------------------- Testing coeffs: [" << type <<
                "]: -----------------------" << endl;

  const coeffs r = nInitChar( type, NULL );

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
  
  type = nRegister( n_Q, nlInitChar); assume( type == n_Q );
  if( Test(type) )
    c ++;

  type = nRegister( n_long_R, ngfInitChar); assume( type == n_long_R );
  if( Test(type) )
    c ++;

  type = nRegister( n_long_C, ngcInitChar); assume( type == n_long_C );
  if( Test(type) )
    c ++;
  
  type = nRegister( n_R, nrInitChar); assume( type == n_R );
  if( Test(type) )
    c ++;


#ifdef HAVE_RINGS
  type = nRegister( n_Z2m, nr2mInitChar); assume( type == n_Z2m );
  if( Test(type) )
    c ++;
#endif


#ifdef HAVE_RINGS
  type = nRegister( n_Zn, nrnInitChar); assume( type == n_Zn );
  if( Test(type) )
    c ++;
/* BUG: 
Program received signal SIGSEGV, Segmentation fault.
0x00007ffff7b91f1b in __gmpz_set () from /usr/lib/libgmp.so.10
(gdb) bt
#0  0x00007ffff7b91f1b in __gmpz_set () from /usr/lib/libgmp.so.10
#1  0x00000000004177a5 in nrnSetExp (m=0, r=0x7ffff7f44ae8) at rmodulon.cc:549
#2  0x0000000000417814 in nrnInitExp (m=0, r=0x7ffff7f44ae8) at rmodulon.cc:556
#3  0x0000000000416122 in nrnInitChar (r=0x7ffff7f44ae8) at rmodulon.cc:34
#4  0x0000000000413278 in nInitChar (t=n_Zn, parameter=0x0) at numbers.cc:146
#5  0x0000000000402f10 in Test (type=n_Zn) at test.cc:47
rmodulon.cc:549       mpz_set(r->nrnModul, r->ringflaga);
(gdb) l
544         r->nrnModul = (int_number) omAllocBin(gmp_nrz_bin);
545         mpz_init(r->nrnModul);
546         nrnMinusOne = (int_number) omAllocBin(gmp_nrz_bin);
547         mpz_init(nrnMinusOne);
548       }
rmodulon.cc:549       mpz_set(r->nrnModul, r->ringflaga);
550       mpz_pow_ui(r->nrnModul, r->nrnModul, nrnExponent);
551       mpz_sub_ui(nrnMinusOne, r->nrnModul, 1);
552     }
553
(gdb) p r->nrnModul
$1 = (int_number) 0x7ffff7f461a8
(gdb) p r->ringflaga
$2 = (int_number) 0x0
*/  

#endif

  
  
  return c;

}
