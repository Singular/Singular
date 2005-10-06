

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
      FFTPrime = (long *) NTL_MALLOC(100, sizeof(long), 0);
      RootTable = (long **) NTL_MALLOC(100, sizeof(long *), 0);
      RootInvTable = (long **) NTL_MALLOC(100, sizeof(long *), 0);
      TwoInvTable = (long **) NTL_MALLOC(100, sizeof(long *), 0);
      FFTPrimeInv = (double *) NTL_MALLOC(100, sizeof(double), 0);
   }
   else if ((index % 100) == 0) {
      FFTPrime = (long *) NTL_REALLOC(FFTPrime, index+100, sizeof(long), 0);
      RootTable = (long **) 
                  NTL_REALLOC(RootTable, index+100, sizeof(long *), 0);
      RootInvTable = (long **) 
                     NTL_REALLOC(RootInvTable, index+100, sizeof(long *), 0);
      TwoInvTable = (long **) 
                    NTL_REALLOC(TwoInvTable, index+100, sizeof(long *), 0);
      FFTPrimeInv = (double *) 
                    NTL_REALLOC(FFTPrimeInv, index+100, sizeof(double), 0);
   }

   if (!FFTPrime || !RootTable || !RootInvTable || !TwoInvTable ||
       !FFTPrimeInv) 
      Error("out of space");

   FFTPrime[index] = q;

   long *rt, *rit, *tit;

   if (!(rt = RootTable[index] = (long*) NTL_MALLOC(mr+1, sizeof(long), 0)))
      Error("out of space");
   if (!(rit = RootInvTable[index] = (long*) NTL_MALLOC(mr+1, sizeof(long), 0)))
      Error("out of space");
   if (!(tit = TwoInvTable[index] = (long*) NTL_MALLOC(mr+1, sizeof(long), 0)))
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
   

static
long RevInc(long a, long k)
{
   long j, m;

   j = k; 
   m = 1L << (k-1);

   while (j && (m & a)) {
      a ^= m;
      m >>= 1;
      j--;
   }
   if (j) a ^= m;
   return a;
}

static
void BitReverseCopy(long *A, const long *a, long k)
{
   static long* mem[NTL_FFTMaxRoot+1];

   long n = 1L << k;
   long* rev;
   long i, j;

   rev = mem[k];
   if (!rev) {
      rev = mem[k] = (long *) NTL_MALLOC(n, sizeof(long), 0);
      if (!rev) Error("out of memory in BitReverseCopy");
      for (i = 0, j = 0; i < n; i++, j = RevInc(j, k))
         rev[i] = j;
   }

   for (i = 0; i < n; i++)
      A[rev[i]] = a[i];
}





/*
 * Our FFT is based on the routine in Cormen, Leiserson, Rivest, and Stein.
 * For very large inputs, it should be relatively cache friendly.
 * The inner loop has been unrolled and pipelined, to exploit any
 * low-level parallelism in the machine.
 */



void FFT(long* A, const long* a, long k, long q, const long* root)

// performs a 2^k-point convolution modulo q

{
   if (k <= 1) {
      if (k == 0) {
	 A[0] = a[0];
	 return;
      }
      if (k == 1) {
	 A[0] = AddMod(a[0], a[1], q);
	 A[1] = SubMod(a[0], a[1], q);
	 return;
      }
   }

   // assume k > 1

   

   static long tab_size = 0;
   static long *wtab = 0;
   static mulmod_precon_t *wqinvtab = 0;

   if (!tab_size) {
      tab_size = k;

      wtab = (long *) NTL_MALLOC(1L << (k-2), sizeof(long), 0);
      wqinvtab = (mulmod_precon_t *) 
                 NTL_MALLOC(1L << (k-2), sizeof(mulmod_precon_t), 0);
      if (!wtab || !wqinvtab) Error("out of space");
   }
   else if (tab_size < k) {
      tab_size = k;

      wtab = (long *) NTL_REALLOC(wtab, 1L << (k-2), sizeof(long), 0);
      wqinvtab = (mulmod_precon_t *) 
                 NTL_REALLOC(wqinvtab, 1L << (k-2), sizeof(mulmod_precon_t), 0);
      if (!wtab || !wqinvtab) Error("out of space");
   }


   double qinv = 1/((double) q);

   wtab[0] = 1;
   wqinvtab[0] = PrepMulModPrecon(1, q, qinv);


   BitReverseCopy(A, a, k);

   long n = 1L << k;

   long s, m, m_half, m_fourth, i, j, t, u, t1, u1, uu, uu1, tt, tt1;

   long w;
   mulmod_precon_t wqinv;

   // s = 1

   for (i = 0; i < n; i += 2) {
      t = A[i + 1];
      u = A[i];
      A[i] = AddMod(u, t, q);
      A[i+1] = SubMod(u, t, q);
   }

   
  
   for (s = 2; s < k; s++) {
      m = 1L << s;
      m_half = 1L << (s-1);
      m_fourth = 1L << (s-2);

      // prepare wtab...

      w = root[s];
      wqinv = PrepMulModPrecon(w, q, qinv);

      for (i = m_half-1, j = m_fourth-1; i >= 0; i -= 2, j--) {
         wtab[i-1] = wtab[j];
         wqinvtab[i-1] = wqinvtab[j];
         wtab[i] = MulModPrecon(wtab[i-1], w, q, wqinv);
         wqinvtab[i] = PrepMulModPrecon(wtab[i], q, qinv);
      }

      for (i = 0; i < n; i+= m) {

          
         t = A[i + m_half];
         u = A[i];
         t1 = MulModPrecon(A[i + 1+ m_half], w, q, wqinv);
         u1 = A[i+1];

         for (j = 0; j < m_half-2; j += 2) {
            tt = MulModPrecon(A[i + j + 2 + m_half], wtab[j+2], q, wqinvtab[j+2]);
            uu = A[i + j + 2];


            tt1 = MulModPrecon(A[i + j + 3+ m_half], wtab[j+3], q, wqinvtab[j+3]);
            uu1 = A[i + j + 3];

            A[i + j] = AddMod(u, t, q);
            A[i + j + m_half] = SubMod(u, t, q);
            A[i + j + 1] = AddMod(u1, t1, q);
            A[i + j + 1 + m_half] = SubMod(u1, t1, q);
            t = tt;
            t1 = tt1;
            u = uu;
            u1 = uu1;
         }


         A[i + j] = AddMod(u, t, q);
         A[i + j + m_half] = SubMod(u, t, q);
         A[i + j + 1] = AddMod(u1, t1, q);
         A[i + j + 1 + m_half] = SubMod(u1, t1, q);



      }
   }


   // s == k...special case

   m = 1L << s;
   m_half = 1L << (s-1);
   m_fourth = 1L << (s-2);


   w = root[s];
   wqinv = PrepMulModPrecon(w, q, qinv);

   // j = 0, 1

   t = A[m_half];
   u = A[0];
   t1 = MulModPrecon(A[1+ m_half], w, q, wqinv);
   u1 = A[1];

   A[0] = AddMod(u, t, q);
   A[m_half] = SubMod(u, t, q);
   A[1] = AddMod(u1, t1, q);
   A[1 + m_half] = SubMod(u1, t1, q);

   for (j = 2; j < m_half; j += 2) {
      t = MulModPrecon(A[j + m_half], wtab[j >> 1], q, wqinvtab[j >> 1]);
      u = A[j];
      t1 = MulModPrecon(A[j + 1+ m_half], wtab[j >> 1], q, 
                        wqinvtab[j >> 1]);
      t1 = MulModPrecon(t1, w, q, wqinv);
      u1 = A[j + 1];

      A[j] = AddMod(u, t, q);
      A[j + m_half] = SubMod(u, t, q);
      A[j + 1] = AddMod(u1, t1, q);
      A[j + 1 + m_half] = SubMod(u1, t1, q);
     
   }
}


NTL_END_IMPL
