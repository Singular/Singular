//static char rcsid[] = "$Id: test.cc,v 1.3 1997-09-12 07:19:34 Singular Exp $";
///////////////////////////////////////////////////////////////////////////////
// Factor - Includes
#include "factor.h"


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

cout << libfac_name  <<", Version " << libfac_version << endl;
//    cout << " Characteristic = "; 
    cin >> charac;
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
//    cout << " f = "; 
    cin >> f;
    cout << f  << " mod " << charac << " = " ;
    Factorlist= Factorize( f,0 );
     cout << Factorlist ;//<< endl;
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
	cerr << "\nList and f differ: f= " << f << "  g= " << g << endl;
    }
}
