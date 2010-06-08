#include "config.h"

#include <auxiliary.h>
#include <coeffs.h>
#include <numbers.h>
#include <output.h>
#include <omalloc.h>


#include <longrat.h>
#include <gnumpc.h>
#include <shortfl.h>


#include <iostream>
using namespace std;


int main()
{
  SPrintStart();
  
  const n_coeffType type = nRegister( n_Q, nlInitChar); assume( type == n_Q );
//  const n_coeffType type = nRegister( n_long_C, ngcInitChar); assume( type == n_long_C );
//  const n_coeffType type = nRegister( n_R, nrInitChar); assume( type == n_R );
  

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
  } else
  {
    // ...
  }


  
  number a = r->cfInit(666, r); 
  number b = r->cfAdd( a, a, r);

  PrintS("a: "); r->cfWrite( a, r );
  PrintS("b: "); r->cfWrite( b, r );
  
  r->cfDelete( &a, r);
  r->cfDelete( &b, r);

  nKillChar( r );

  cout << "OUTPUT BUFFER: " << SPrintEnd();
  
  return 0;
}
