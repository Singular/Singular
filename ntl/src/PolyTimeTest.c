
#include <NTL/ZZ_pX.h>

#include <stdio.h>

NTL_CLIENT

void print_flag()
{

#ifdef NTL_AVOID_BRANCHING
printf("AVOID_BRANCHING ");
#endif

#ifdef NTL_TBL_REM
printf("TBL_REM ");
#endif

#ifdef NTL_FFT_PIPELINE
printf("FFT_PIPELINE ");
#endif

printf("\n");

}


int main()
{
   _ntl_gmp_hack = 0;

#ifdef NTL_LONG_LONG

#ifndef NTL_LONG_LONG_TYPE
#define NTL_LONG_LONG_TYPE long long
#endif

   if (sizeof(NTL_LONG_LONG_TYPE) != 2*sizeof(long)) {
      printf("999999999999999 ");
      print_flag();
      return 0;
   }

#endif

   long n, k;

   n = 200;
   k = 10*NTL_ZZ_NBITS;

   ZZ p;

   GenPrime(p, k);


   ZZ_p::init(p);         // initialization

   ZZ_pX f, g, h, r1, r2, r3;

   random(g, n);    // g = random polynomial of degree < n
   random(h, n);    // h =             "   "
   random(f, n);    // f =             "   "

   // SetCoeff(f, n);  // Sets coefficient of X^n to 1
   
   ZZ_p lc;

   do {
      random(lc);
   } while (IsZero(lc));

   SetCoeff(f, n, lc);

   // For doing arithmetic mod f quickly, one must pre-compute
   // some information.

   ZZ_pXModulus F;
   build(F, f);

   PlainMul(r1, g, h);  // this uses classical arithmetic
   PlainRem(r1, r1, f);

   MulMod(r2, g, h, F);  // this uses the FFT

   MulMod(r3, g, h, f);  // uses FFT, but slower

   // compare the results...

   if (r1 != r2) {
      printf("999999999999999 ");
      print_flag();
      return 0;
   }
   else if (r1 != r3) {
      printf("999999999999999 ");
      print_flag();
      return 0;
   }

   double t;
   long i;
   long iter;

   n = 1024;
   k = 1024;
   RandomLen(p, k);

   ZZ_p::init(p);

   ZZ_pX j1, j2, j3;

   random(j1, n);
   random(j2, n);

   mul(j3, j1, j2);

   iter = 1;

   do {
     t = GetTime();
     for (i = 0; i < iter; i++)
        FFTMul(j3, j1, j2);
     t = GetTime() - t;
     iter = 2*iter;
   } while(t < 1);

   iter = iter/2;

   t = floor((t/iter)*1e9);

   if (t < 0 || t >= 1e15)
      printf("999999999999999 ");
   else
      printf("%015.0f ", t);

   print_flag();

   return 0;
}
