

#ifndef NTL_FFT__H
#define NTL_FFT__H

#include <NTL/vec_long.h>
#include <NTL/ZZ.h>

NTL_OPEN_NNS

#define NTL_FFTFudge (4)
// This constant is used in selecting the correct
// number of FFT primes for polynomial multiplication
// in ZZ_pX and zz_pX.  Set at 4, this allows for
// two FFT reps to be added or subtracted once,
// before performing CRT, and leaves a reasonable margin for error.
// Don't change this!

#define NTL_FFTMaxRootBnd (NTL_SP_NBITS-2)
// Absolute maximum root bound for FFT primes.
// Don't change this!

#if (25 <= NTL_FFTMaxRootBnd)
#define NTL_FFTMaxRoot (25)
#else
#define NTL_FFTMaxRoot  NTL_FFTMaxRootBnd
#endif
// Root bound for FFT primes.  Held to a maximum
// of 25 to avoid large tables and excess precomputation,
// and to keep the number of FFT primes needed small.
// This means we can multiply polynomials of degree less than 2^24.  
// This can be increased, with a slight performance penalty.



extern long NumFFTPrimes;

extern long *FFTPrime;
extern long **RootTable;
extern long **RootInvTable;
extern long **TwoInvTable;
extern double *FFTPrimeInv;

/****************

   RootTable[i][j] = w^{2^{MaxRoot-j}},
                     where w is a primitive 2^MaxRoot root of unity
                     for FFTPrime[i]

   RootInvTable[i][j] = 1/RootTable[i][j] mod FFTPrime[i]

   TwoInvTable[i][j] = 1/2^j mod FFTPrime[i]

   These tables are allocated and initialized at run-time
   with UseFFTPrime(i).

*****************/


long CalcMaxRoot(long p);
// calculates max power of two supported by this FFT prime.

void UseFFTPrime(long index);
// allocates and initializes information for FFT prime


void FFT(long* A, const long* a, long k, long q, const long* root);
// the low-level FFT routine.
// computes a 2^k point FFT modulo q, using the table root for the roots.

NTL_CLOSE_NNS

#endif
