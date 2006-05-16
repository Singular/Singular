//static char rcsid[] = "$Id: test.cc,v 1.4 2006-05-16 14:46:48 Singular Exp $";
///////////////////////////////////////////////////////////////////////////////
// Factor - Includes
#include "factor.h"
#ifdef HAVE_IOSTREAM
#include <iostream>
#define CERR std::cerr
#define COUT std::cout
#define CIN std::cin
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define CERR cerr
#define COUT cout
#define CIN cin
#endif



void
setVarNames ( char * vnames )
{
    int i, n = strlen( vnames );
    Variable dummy;
    for ( i = 0; i < n; i++ )
        dummy = Variable( i+1, vnames[i] );
}

main()
{
    CanonicalForm f,g;
    int charac;
    CFFList Factorlist;
    CFFactor copy;

COUT << libfac_name  <<", Version " << libfac_version << "\n";
//    COUT << " Characteristic = "; 
    CIN >> charac;
    setCharacteristic(charac);
    On(SW_SYMMETRIC_FF);
    On(SW_USE_EZGCD);
//////////////////////////////////////////////////////////////
//  char vnames[30];
////  cout << " vnames = "; 
//  cin >> vnames;
//
//  setVarNames( vnames );
//
//////////////////////////////////////////////////////////////
//    COUT << " f = "; 
    CIN >> f;
    COUT << f  << " mod " << charac << " = " ;
    Factorlist= Factorize( f,0 );
     COUT << Factorlist ;//<< "\n";
// Ueberpruefen:
    g=1;
    for ( CFFListIterator i=Factorlist ; i.hasItem(); i++){
	copy = i.getItem();
	if ( getNumVars(copy.factor()) > 0 )
	     for ( int j=1; j<=copy.exp(); j++)
	     g *= copy.factor();
	else g *= copy.factor();
    }
    if ( (f-g) != f.genZero() ){
	CERR << "\nList and f differ: f= " << f << "  g= " << g << "\n";
    }
}
