
#include <NTL/ZZ_pX.h>

#include <stdio.h>

NTL_CLIENT


double clean_data(double *t)
{
   double x, y, z;
   long i, ix, iy, n;

   x = t[0]; ix = 0;
   y = t[0]; iy = 0;

   for (i = 1; i < 5; i++) {
      if (t[i] < x) {
         x = t[i];
         ix = i;
      }
      if (t[i] > y) {
         y = t[i];
         iy = i;
      }
   }

   z = 0; n = 0;
   for (i = 0; i < 5; i++) {
      if (i != ix && i != iy) z+= t[i], n++;
   }

   z = z/n;  

   return z;
}

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


   if (sizeof(NTL_LL_TYPE) < 2*sizeof(long)) {
      printf("999999999999999 ");
      print_flag();
      return 0;
   }

#endif

   long i, k;

   
   k = 10*NTL_ZZ_NBITS;

   for (i = 0; i < 10000; i++) {
      ZZ a, b, c, d;
      long da = RandomBnd(k);
      long db = RandomBnd(k);
      long dc = RandomBnd(k);
      long dd = RandomBnd(k);
      RandomLen(a, da);  RandomLen(b, db);  RandomLen(c, dc);  RandomLen(d, dd);

      if ((a + b)*(c + d) != c*a + d*a + c*b + d*b) {
	 printf("999999999999999 ");
	 print_flag();
	 return 0;
      }
   }

   

   for (i = 0; i < 10000; i++) {
      ZZ a, b, c;
      
      long da = RandomBnd(k);
      long db = RandomBnd(k);
      long dc = RandomBnd(k) + 2;
      
      RandomLen(a, da);  RandomLen(b, db);  RandomLen(c, dc); 

      if ( ( a * b ) % c != ((a % c) * (b % c)) % c ) {
	 printf("999999999999999 ");
	 print_flag();
	 return 0;
      }
   }

   k = 16*NTL_ZZ_NBITS;

   ZZ x1, x2, x3;
   double t;
   long j;

   RandomLen(x1, k);
   RandomLen(x2, k);
   

   long iter;

   mul(x3, x1, x2);

   iter = 1;

   do {
     t = GetTime();
     for (i = 0; i < iter; i++) {
        for (j = 0; j < 500; j++) mul(x3, x1, x2);
      }
     t = GetTime() - t;
     iter = 2*iter;
   } while(t < 1);


   iter = iter/2;
   iter = long((2/t)*iter) + 1;

   double tvec[5];
   long w;

   for (w = 0; w < 5; w++) {
     t = GetTime();
     for (i = 0; i < iter; i++) {
        for (j = 0; j < 500; j++) mul(x3, x1, x2);
      }
     t = GetTime() - t;
     tvec[w] = t;
   }

   t = clean_data(tvec);

   t = floor((t/iter)*1e14);

   if (t < 0 || t >= 1e15)
      printf("999999999999999 ");
   else
      printf("%015.0f ", t);

   printf(" [%ld] ", iter);

   print_flag();

   return 0;
}
