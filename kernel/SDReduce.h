#include <SDkutil.h>
#ifndef SDREDUCE_H
#define SDREDUCE_H

namespace ShiftDVec
{
  int redHomog( LObject* h, ::kStrategy strategy );

  namespace Reduce
  {
    int red_get_opt_len( uint* shift,
                         kStrategy strat, int i,int li,
                         unsigned long not_sev, LObject* h );
    void debug_red_with_print( TObject* red, TObject* with );
    void debug_red_to_print( LObject* to );
    void red_reduce_with( LObject* red, TObject* with,
                          uint shift, kStrategy strat );
    unsigned long after_red( LObject* h );
    int red_with_S( LObject* h, kStrategy strat, int* pass );
  }
}

#endif
