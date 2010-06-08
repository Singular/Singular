#include "config.h"

#include <auxiliary.h>
#include <coeffs.h>
#include <numbers.h>
#include <output.h>
#include <omalloc.h>


//#include <longrat.h>
#include <gnumpc.h>


#include <iostream>
using namespace std;


int main()
{
  const n_coeffType type = n_long_C; // n_Q;

  const coeffs r = nInitChar( type, NULL );

  assume( r != NULL );

  assume( r->cfInit != NULL );
  assume( r->cfWrite != NULL );
  assume( r->cfAdd != NULL );
  assume( r->cfDelete != NULL );

  assume( getCoeffType(r) == type );

  if( type == n_Q )
  {
    
/*
    assume( r->cfInit == nlInit );
    assume( r->cfWrite == nlWrite );
    assume( r->cfAdd == nlAdd );
    assume( r->cfDelete == nlDelete );    
*/
    }
  else if( type == n_long_C )
  {
    assume( r->cfInit == ngcInit );
    assume( r->cfWrite == ngcWrite );
    assume( r->cfAdd == ngcAdd );
    assume( r->cfDelete == ngcDelete );    
  }


  
  number a = r->cfInit(666, r); 
  r->cfWrite( a, r );
  number b = r->cfAdd( a, a, r);
  r->cfWrite( b, r );
  r->cfDelete( &a, r);
  r->cfDelete( &b, r);

  
  
  return 0;
}
