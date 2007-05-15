//static char rcsid[] = "@(#) $Id: test.cc,v 1.3 2007-05-15 14:46:49 Singular Exp $";
///////////////////////////////////////////////////////////////////////////////
// CLAP - Includes
//       (x+y+z)*(x+y-z)*(x-y+z)*(x-y-z)*x*y;
#include <clap.h>
// Factor - Includes
#include "tmpl_inst.h"
#include "Factor.h"
#include "SqrFree.h"

main()
{
    CanonicalForm f,g;
    int charac;
    CFFList Factorlist;
    CFFactor copy;

//    cout << " Characteristic = "; 
    cin >> charac;
    setCharacteristic(charac);
    On(SW_SYMMETRIC_FF);
    g=1;
//    cout << " f = "; 
    cin >> f;
//    cout << " f = " << f  << endl;
//    cout << " f(SqrFree)= " << endl;
//    cout << SqrFree(f) << endl << endl;
//cout << "gcd(f,f.deriv())= gcd(" << f << "," << f.deriv() << ")= " << gcd(f,f.deriv()) << endl;
//cout << "gcd(f,f.deriv())= gcd(" << f << "," << f.deriv() << ")= " << gcd(f,f.deriv()) << endl;
        Factorlist = Factorize( f );
        cout << Factorlist ;//<< endl;
// Ueberpruefen:
    for ( CFFListIterator i=Factorlist ; i.hasItem(); i++){
	copy = i.getItem();
	if ( getNumVars(copy.factor()) > 0 )
	     for ( int j=1; j<=copy.exp(); j++)
	     g *= copy.factor();
	else g *= copy.factor();
    }
    if ( (f-g) != f.genZero() ){
	cerr << "\nList and f differ: f= " << f << "  g= " << g << endl;
	cerr << "List= " << Factorlist << endl;
    }
}
