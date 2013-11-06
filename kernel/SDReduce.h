#include <SDkutil.h>
#ifndef SDREDUCE_H
#define SDREDUCE_H

namespace ShiftDVec
{
  int redHomog( LObject* h, ::kStrategy strategy );

  namespace Reduce
  {
    int red_get_opt_len( uint* shift, kStrategy strat,
                         int i,int li, LObject* h      );
    void debug_red_with_print( TObject* red, TObject* with );
    void debug_red_to_print( LObject* to );
    void red_reduce_with( LObject* red, TObject* with,
                          uint shift, kStrategy strat );
    unsigned long after_red( LObject* h );
    int red_with_US( LObject* h, kStrategy strat, int* pass );
    int kFindDivisibleByInT
      ( const TSet &T, int tlen,
        const unsigned long* sevT, LObject * L, 
        uint & shift, SD::kStrategy strat, const int start );
    int kFindDivisibleByInT( const TSet &T, int tlen,
                             const unsigned long* sevT,
                             LObject * L, uint & shift,
                             SD::kStrategy strat,
                             const int start = 0 );
    int kFindDivisibleByInUT( LObject* h, uint& shift,
                              SD::kStrategy strat );
    int kFindDivisibleByInUS( LObject* h, uint& shift,
                              SD::kStrategy strat );
    TObject * kFindDivisibleByInS
      ( SD::kStrategy strat, int pos,
        LObject* L, TObject* T, 
        uint & shift, int lV, int uptodeg, long ecart );
  }
}

#endif
