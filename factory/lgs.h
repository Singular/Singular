/* ======================================================================
    Dateiname:          lgs.h

    Beschreibung: Enthält alle Definition und Funktionen zum
                  Umgang mit dem linearen Gleichungssystemen.
   =================================================================== */

#ifndef lgs__H
#define lgs__H

 

#include "cf_factory.h"
#ifdef HAVE_BIFAC

  
class LGS
{
////////////////////////////////////////////////////////////////
  public:
////////////////////////////////////////////////////////////////  

  // === KONST-/ DESTRUKTOREN ====
  LGS         (int rows, int columns, bool inverse =false );  // Konstruktor
  virtual ~LGS( void );  // DESTRUKTOR

  // === Funktionen =======
  bool   new_row( const CFMatrix Z, const CanonicalForm b =0);

  int       rank( void );
  int       corank( void );
  CFMatrix  GetSolutionVector( void );
  CFMatrix  GetKernelBasis( void );
  void      reset(void);
  void      inverse( CFMatrix& I);
  void      print(void);


////////////////////////////////////////////////////////////////
 private:
////////////////////////////////////////////////////////////////

  // === Funktionen =======
  bool     lin_dep(void);
  void     reduce(int fix, int row);
  int      ErgCol(int row, int basis[]);

  // === Variable =======
  CFMatrix A, b;       // Matrices of the linear equation system

  bool KERN;        // compute the kernel
  bool INVERSE;     // compute at once the inverse matrix
  int max_columns;  // Für so viele Spalten wurde Platz alloziiert.
  int max_rows;     // Für so viele Zeilen wurde Platz alloziiert.
  int now_row;      // Soviele der Zeilen wurden bisher benutzt.
  int* pivot;      // pivot column of the i-te row
};

#endif
// =============== Ende der Datei 'lgs.h' ============================

#endif
