
#include <NTL/ZZ_pX.h>

#include <stdio.h>

NTL_CLIENT

void print_flag()
{

#if defined(NTL_LONG_LONG)
printf("LONG_LONG\n");
#elif defined(NTL_AVOID_FLOAT)
printf("AVOID_FLOAT\n");
#else
printf("DEFAULT\n");
#endif

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

   ZZ x1, x2, x3, x4;
   double t;
   long i;

   RandomLen(x1, 512);
   RandomBnd(x2, x1);
   RandomBnd(x3, x1);

   long iter;

   mul(x4, x2, x3);

   iter = 1;

   do {
     t = GetTime();
     for (i = 0; i < iter; i++)
        mul(x4, x2, x3);
     t = GetTime() - t;
     iter = 2*iter;
   } while(t < 1);

   iter = iter/2;

   t = floor((t/iter)*1e14);

   if (t < 0 || t >= 1e15)
      printf("999999999999999 ");
   else
      printf("%015.0f ", t);

   print_flag();

   return 0;
}
