

#include <NTL/FFT.h>

#include <NTL/new.h>

NTL_START_IMPL

long NumFFTPrimes = 0;

long *FFTPrime = 0;
long **RootTable = 0;
long **RootInvTable = 0;
long **TwoInvTable = 0;
double *FFTPrimeInv = 0;


static
long IsFFTPrime(long n, long& w)
{
   long  m, x, y, z;
   long j, k;

   if (n % 3 == 0) return 0;

   if (n % 5 == 0) return 0;

   if (n % 7 == 0) return 0;
   
   m = n - 1;
   k = 0;
   while ((m & 1) == 0) {
      m = m >> 1;
      k++;
   }

   for (;;) {
      x = RandomBnd(n);

      if (x == 0) continue;
      z = PowerMod(x, m, n);
      if (z == 1) continue;

      x = z;
      j = 0;
      do {
         y = z;
         z = MulMod(y, y, n);
         j++;
      } while (j != k && z != 1);

      if (z != 1 || y !=  n-1) return 0;

      if (j == k) 
         break;
   }

   /* x^{2^k} = 1 mod n, x^{2^{k-1}} = -1 mod n */

   long TrialBound;

   TrialBound = m >> k;
   if (TrialBound > 0) {
      if (!ProbPrime(n, 5)) return 0;
   
      /* we have to do trial division by special numbers */
   
      TrialBound = SqrRoot(TrialBound);
   
      long a, b;
   
      for (a = 1; a <= TrialBound; a++) {
         b = (a << k) + 1;
         if (n % b == 0) return 0; 
      }
   }

   /* n is an FFT prime */

   for (j = NTL_FFTMaxRoot; j < k; j++)
      x = MulMod(x, x, n);

   w = x;
   return 1;
}


static
void NextFFTPrime(long& q, long& w)
{
   static long m = NTL_FFTMaxRootBnd + 1;
   static long k = 0;

   long t, cand;

   for (;;) {
      if (k == 0) {
         m--;
         if (m < 5) Error("ran out of FFT primes");
         k = 1L << (NTL_SP_NBITS-m-2);
      }

      k--;

      cand = (1L << (NTL_SP_NBITS-1)) + (k << (m+1)) + (1L << m) + 1;

      if (!IsFFTPrime(cand, t)) continue;
      q = cand;
      w = t;
      return;
   }
}


long CalcMaxRoot(long p)
{
   p = p-1;
   long k = 0;
   while ((p & 1) == 0) {
      p = p >> 1;
      k++;
   }

   if (k > NTL_FFTMaxRoot)
      return NTL_FFTMaxRoot;
   else
      return k; 
}


void UseFFTPrime(long index)
{
   if (index < 0 || index > NumFFTPrimes)
      Error("invalid FFT prime index");

   if (index < NumFFTPrimes) return;

   long q, w;

   NextFFTPrime(q, w);

   long mr = CalcMaxRoot(q);

   // tables are allocated in increments of 100

   if (index == 0) { 
      FFTPrime = (long *) malloc(sizeof(long)*100);
      RootTable = (long **) malloc(sizeof(long *)*100);
      RootInvTable = (long **) malloc(sizeof(long *)*100);
      TwoInvTable = (long **) malloc(sizeof(long *)*100);
      FFTPrimeInv = (double *) malloc(sizeof(double)*100);
   }
   else if ((index % 100) == 0) {
      FFTPrime = (long *) realloc(FFTPrime, sizeof(long)*(index+100));
      RootTable = (long **) realloc(RootTable, sizeof(long *)*(index+100));
      RootInvTable = (long **) 
                     realloc(RootInvTable, sizeof(long *)*(index+100));
      TwoInvTable = (long **) realloc(TwoInvTable, sizeof(long *)*(index+100));
      FFTPrimeInv = (double *) realloc(FFTPrimeInv, sizeof(double)*(index+100));
   }

   if (!FFTPrime || !RootTable || !RootInvTable || !TwoInvTable ||
       !FFTPrimeInv) 
      Error("out of space");

   FFTPrime[index] = q;

   long *rt, *rit, *tit;

   if (!(rt = RootTable[index] = (long *)malloc(sizeof(long)*(mr+1))))
      Error("out of space");
   if (!(rit = RootInvTable[index] = (long *)malloc(sizeof(long)*(mr+1))))
      Error("out of space");
   if (!(tit = TwoInvTable[index] = (long *)malloc(sizeof(long)*(mr+1))))
      Error("out of space");

   long j;
   long t;

   rt[mr] = w;
   for (j = mr-1; j >= 0; j--)
      rt[j] = MulMod(rt[j+1], rt[j+1], q);

   rit[mr] = InvMod(w, q);
   for (j = mr-1; j >= 0; j--)
      rit[j] = MulMod(rit[j+1], rit[j+1], q);

   t = InvMod(2, q);
   tit[0] = 1;
   for (j = 1; j <= mr; j++)
      tit[j] = MulMod(tit[j-1], t, q);

   FFTPrimeInv[index] = 1/double(q);

   NumFFTPrimes++;
}
   

long RevInc(long a, long k)
{
   long j, m;
   j = k; m = 1L << (k-1);
   while (j && (m & a)) {
      a ^= m;
      m >>= 1;
      j--;
   }
   if (j) a ^= m;
   return a;
}

void BitReverseCopy(long *A, const long *a, long k)

{
   static long* mem[NTL_FFTMaxRoot+1];

   long n = 1L << k;
   long* rev;
   long i, j;

   rev = mem[k];
   if (!rev) {
      rev = mem[k] = NTL_NEW_OP long[n];
      if (!rev) Error("out of memory in BitReverseCopy");
      for (i = 0, j = 0; i < n; i++, j = RevInc(j, k))
         rev[i] = j;
   }

   for (i = 0; i < n; i++)
      A[rev[i]] = a[i];
}


#ifdef NTL_FFT_PIPELINE

/*****************************************************

   This version of the FFT is written with an explicit
   "software pipeline", which sometimes speeds things up.

*******************************************************/

void FFT(long* A, const long* a, long k, long q, const long* root)

// performs a 2^k-point convolution modulo q

{
   if (k == 0) {
      A[0] = a[0];
      return;
   }

   if (k == 1) {
      A[0] = AddMod(a[0], a[1], q);
      A[1] = SubMod(a[0], a[1], q);
      return;
   }

   // assume k > 1

   long n = 1L << k;
   long s, m, m2, j;
   long t, u, v, w, z, tt;
   long *p1, *p2, *ub, *ub1;
   double qinv = ((double) 1)/((double) q);
   double wqinv, zqinv;

   BitReverseCopy(A, a, k);

   ub = A+n;

   p2 = A;
   while (p2 < ub) {
      u = *p2;
      v = *(p2+1);
      *p2 = AddMod(u, v, q);
      *(p2+1) = SubMod(u, v, q);
      p2 += 2;
   }

   for (s = 2; s < k; s++) {
      m = 1L << s;
      m2 = m >> 1;

      p2 = A;
      p1 = p2 + m2;
      while (p2 < ub) {
         u = *p2;
         v = *p1;
         *p2 = AddMod(u, v, q);
         *p1 = SubMod(u, v, q);
         p1 += m;
         p2 += m;
      }

      z = root[s];
      w = z;
      for (j = 1; j < m2; j++) {
         wqinv = ((double) w)*qinv;
         p2 = A + j;
         p1 = p2 + m2;

         ub1 = ub-m;

         u = *p2;
         t = MulMod2(*p1, w, q, wqinv);

         while (p2 < ub1) {
            tt = MulMod2(*(p1+m), w, q, wqinv);
            *p2 = AddMod(u, t, q);
            *p1 = SubMod(u, t, q);
            p1 += m;
            p2 += m;
            u = *p2;
            t = tt;
         }
         *p2 = AddMod(u, t, q);
         *p1 = SubMod(u, t, q);
         
         w = MulMod2(z, w, q, wqinv);
      }
   }

   m2 = n >> 1;
   z = root[k];
   zqinv = ((double) z)*qinv;
   w = 1;
   p2 = A;
   p1 = A + m2;
   m2--;
   u = *p2;
   t = *p1;
   while (m2) {
      w = MulMod2(w, z, q, zqinv);
      tt = MulMod(*(p1+1), w, q, qinv);
      *p2 = AddMod(u, t, q);
      *p1 = SubMod(u, t, q);
      p2++;
      p1++;
      u = *p2;
      t = tt;
      m2--;
   }
   *p2 = AddMod(u, t, q);
   *p1 = SubMod(u, t, q);
}



#else


/*****************************************************

   This version of the FFT has no "software pipeline".

******************************************************/



void FFT(long* A, const long* a, long k, long q, const long* root)

// performs a 2^k-point convolution modulo q

{
   if (k == 0) {
      A[0] = a[0];
      return;
   }

   if (k == 1) {
      A[0] = AddMod(a[0], a[1], q);
      A[1] = SubMod(a[0], a[1], q);
      return;
   }

   // assume k > 1

   long n = 1L << k;
   long s, m, m2, j;
   long t, u, v, w, z;
   long *p1, *p2, *ub;
   double qinv = ((double) 1)/((double) q);
   double wqinv, zqinv;

   BitReverseCopy(A, a, k);

   ub = A+n;

   p2 = A;
   while (p2 < ub) {
      u = *p2;
      v = *(p2+1);
      *p2 = AddMod(u, v, q);
      *(p2+1) = SubMod(u, v, q);
      p2 += 2;
   }

   for (s = 2; s < k; s++) {
      m = 1L << s;
      m2 = m >> 1;

      p2 = A;
      p1 = p2 + m2;
      while (p2 < ub) {
         u = *p2;
         v = *p1;
         *p2 = AddMod(u, v, q);
         *p1 = SubMod(u, v, q);
         p1 += m;
         p2 += m;
      }

      z = root[s];
      w = z;
      for (j = 1; j < m2; j++) {
         wqinv = ((double) w)*qinv;
         p2 = A + j;
         p1 = p2 + m2;
         while (p2 < ub) {
            u = *p2;
            v = *p1;
            t = MulMod2(v, w, q, wqinv);
            *p2 = AddMod(u, t, q);
            *p1 = SubMod(u, t, q);
            p1 += m;
            p2 += m;
         }
         w = MulMod2(z, w, q, wqinv);
      }
   }

   m2 = n >> 1;
   z = root[k];
   zqinv = ((double) z)*qinv;
   w = 1;
   p2 = A;
   p1 = A + m2;
   for (j = 0; j < m2; j++) {
      u = *p2;
      v = *p1;
      t = MulMod(v, w, q, qinv);
      *p2 = AddMod(u, t, q);
      *p1 = SubMod(u, t, q);
      w = MulMod2(w, z, q, zqinv);
      p2++;
      p1++;
   }
}

#endif

NTL_END_IMPL
