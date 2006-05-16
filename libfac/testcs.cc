// $Id: testcs.cc,v 1.3 2006-05-16 14:46:48 Singular Exp $

#include <factory.h>
#include <factor.h>
#ifdef HAVE_IOSTREAM
#include <iostream>
#define COUT std::cout
#define CIN std::cin
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define COUT cout
#define CIN cin
#endif


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
  int reord=0; // use strategy for reordering?
  int charac;
  COUT << " Characteristic = "; CIN >> charac;
  setCharacteristic(charac);
  On(SW_SYMMETRIC_FF);
  char vnames[30];
  COUT << " vnames = "; CIN >> vnames;

  setVarNames( vnames );

  CFList F1,F2;
  ListCFList Q;
  CanonicalForm f;
  CFListIterator I ;
  int i = 1;
  PremForm Remembern;
  Varlist betterorder;

  COUT << " f[" << i << "] = "; cin >> f;
  while ( f != 0 ) {
    F1.append( f ); i++;
    COUT << " f[" << i << "] = "; cin >> f;
  }
  F2=F1;
  COUT << "\n";

  COUT << "F = " << F1 << "\n";
  if (reord){
    betterorder= neworder(F1);
    COUT << "new orderd vars: " << betterorder << "\n";
    F1=reorder(betterorder,F1);
    COUT << "new F= " << F1 << "\n";
  }
//   COUT << "\nUsing Modified CharSet:" << "\n";
//   F2 = MCharSetN( F1, Remembern );
//   COUT << "MCsN(F) = " << F2 << "\n";
//   COUT << "Factors removed: " << Remembern.FS2 << "\n";
//   COUT << "Possible Factors considered: " << Remembern.FS1 << "\n";
//   for ( I = F2; I.hasItem(); I++ )
//     COUT << "  vars: " << getVars( I.getItem() ) << "\n";
  COUT << "\nUsing IrrCharSeries:" << "\n";
  Q= IrrCharSeries( F1 );
  COUT << "ics(F) = " << Q << "\n";
  if (reord){
    Q=reorder(betterorder,Q);
    COUT << "reorderd ics(F) = " << Q << "\n";
  }
}

