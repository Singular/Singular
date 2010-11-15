/* ======================================================================
    Dateiname:          bifac.h
    Autor:              Holger L. Cröni
    Bearbeitung:        29. August 2002


    Beschreibung: Klasse zur Faktorisierung bivariater Polynome
   =================================================================== */

#ifndef bifac__H
#define bifac__H


#include <time.h>
#include <factoryconf.h>
#ifdef HAVE_BIFAC

#include "lgs.h"
#include "bifacConfig.h"

// === IO-Streams ===
#ifndef NOSTREAMIO
#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif
#ifdef HAVE_IOSTREAM
#include <fstream>
#include <iostream>
#elif defined(HAVE_IOSTREAM_H)
#include <fstream.h>
#include <iostream.h>
#endif
#include <iomanip.h>
#endif

/*BEGINPUBLIC*/

CFFList AbsFactorize( const CanonicalForm  & a );

class BIFAC
{
////////////////////////////////////////////////////////////////
  public:
////////////////////////////////////////////////////////////////

  // === KONST-/ DESTRUKTOREN ====
  BIFAC         ( void );  // Konstruktor
  virtual ~BIFAC( void );  // DESTRUKTOR

  // === Funktionen =======
  void     bifac(CanonicalForm f, bool absolute=true);
  CFFList  getFactors( void ){ return  gl_RL; };


////////////////////////////////////////////////////////////////
 private:
////////////////////////////////////////////////////////////////

  // === Funktionen =======
  void   passedTime();
  void   biGanzMachen(  CanonicalForm & f );
  void   biNormieren( CanonicalForm & f ) ;
  void   convertResult(  CanonicalForm & f, int ch, int sw);
  int    findCharacteristic(CanonicalForm f);
//  void   matrix_drucken( CFMatrix M );
  long int  anz_terme(  CanonicalForm & f );

  CFList matrix2basis(CFMatrix A, int dim, int m, int n, CanonicalForm f);
  CFList basisOfG(CanonicalForm f);
  CFMatrix createA (CFList G, CanonicalForm f);
  CanonicalForm  create_g    (CFList G);
  CFList         createRg    (CFList G, CanonicalForm f);
  CFList         createEg    (CFList G, CanonicalForm f);
  CFList         createEgUni (CFList G, CanonicalForm f);

  void     unifac(CanonicalForm f, int grad);
  CanonicalForm RationalFactor (CanonicalForm phi, CanonicalForm f, \
                                CanonicalForm fx, CanonicalForm g);
  void   RationalFactorizationOnly (CFFList Phis, CanonicalForm f, CanonicalForm g);
  CFList getAbsoluteFactors (CanonicalForm f1, CanonicalForm phi);
  void   AbsoluteFactorization (CFFList Phis, CanonicalForm f, CanonicalForm g);
  void   bifacSqrFree( CanonicalForm f );
  void   bifacMain(CanonicalForm f);


  // === Variable =======
  CFFList gl_RL;    // where to store the rational factorization
  CFList  gl_AL;    // where to store the absolute factorization
  bool   absolute;  // Compute an absolute factorization as well?
  int    exponent;  //
};

/*ENDPUBLIC*/

#endif
// =============== Ende der Datei 'bifac.h' ============================

#endif
