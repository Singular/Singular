// $Id: testcs.cc,v 1.2 1997-06-09 15:55:57 Singular Exp $

#include <factory.h>
#include "charset.h"
#include "reorder.h"

// testing: Characteristic Sets
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
  int reord=1; // use strategy for reordering?
  int charac;
  cout << " Characteristic = "; cin >> charac;
  setCharacteristic(charac);
  On(SW_SYMMETRIC_FF);
  char vnames[30];
  cout << " vnames = "; cin >> vnames;

  setVarNames( vnames );

  CFList F1,F2;
  ListCFList Q;
  CanonicalForm f;
  CFListIterator I ;
  int i = 1;
  PremForm Remembern;
  Varlist betterorder;

  cout << " f[" << i << "] = "; cin >> f;
  while ( f != 0 ) {
    F1.append( f ); i++;
    cout << " f[" << i << "] = "; cin >> f;
  }
  F2=F1;
  cout << endl;

  cout << "F = " << F1 << endl;
  if (reord){
    betterorder= neworder(F1);
    cout << "new orderd vars: " << betterorder << endl;
    F1=reorder(betterorder,F1);
    cout << "new F= " << F1 << endl;
  }
//   cout << "\nUsing Modified CharSet:" << endl;
//   F2 = MCharSetN( F1, Remembern );
//   cout << "MCsN(F) = " << F2 << endl;
//   cout << "Factors removed: " << Remembern.FS2 << endl;
//   cout << "Possible Factors considered: " << Remembern.FS1 << endl;
//   for ( I = F2; I.hasItem(); I++ )
//     cout << "  vars: " << getVars( I.getItem() ) << endl;
  cout << "\nUsing IrrCharSeries:" << endl;
  Q= IrrCharSeries( F1 );
  cout << "ics(F) = " << Q << endl;
  if (reord){
    Q=reorder(betterorder,Q);
    cout << "reorderd ics(F) = " << Q << endl;
  }
}

