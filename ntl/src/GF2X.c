
#include <NTL/GF2X.h>
#include <NTL/vec_long.h>

#include <NTL/new.h>

NTL_START_IMPL

long GF2X::HexOutput = 0;


void GF2X::SetMaxLength(long n)
{
   if (n < 0) Error("GF2X::SetMaxLength: negative length");
   if (NTL_OVERFLOW(n, 1, 0))
      Error("GF2X::SetMaxLength: excessive length");
   long w = (n + NTL_BITS_PER_LONG - 1)/NTL_BITS_PER_LONG;
   xrep.SetMaxLength(w);
}

GF2X::GF2X(INIT_SIZE_TYPE, long n)
{
   SetMaxLength(n);
}



const GF2X& GF2X::zero()
{
   static GF2X z;
   return z;
}

void GF2X::normalize()
{
   long n;
   const _ntl_ulong *p;

   n = xrep.length();
   if (n == 0) return;
   p = xrep.elts() + n;
   while (n > 0 && (*--p) == 0) {
      n--;
   }
   xrep.QuickSetLength(n);
}

long IsZero(const GF2X& a) 
   { return a.xrep.length() == 0; }

long IsOne(const GF2X& a)
   { return a.xrep.length() == 1 && a.xrep[0] == 1; }







long IsX(const GF2X& a)
{
   return a.xrep.length() == 1 && a.xrep[0] == 2;
}

GF2 coeff(const GF2X& a, long i)
{
   if (i < 0) return to_GF2(0);
   long wi = i/NTL_BITS_PER_LONG;
   if (wi >= a.xrep.length()) return to_GF2(0);
   long bi = i - wi*NTL_BITS_PER_LONG;

   return to_GF2((a.xrep[wi] & (1UL << bi)) != 0);
}

GF2 LeadCoeff(const GF2X& a)
{
   if (IsZero(a))
      return to_GF2(0);
   else
      return to_GF2(1);
}

GF2 ConstTerm(const GF2X& a)
{
   if (IsZero(a))
      return to_GF2(0);
   else
      return to_GF2((a.xrep[0] & 1) != 0);
}


void set(GF2X& x)
{
   x.xrep.SetLength(1);
   x.xrep[0] = 1;
}

void SetX(GF2X& x)
{
   x.xrep.SetLength(1);
   x.xrep[0] = 2;
}

void SetCoeff(GF2X& x, long i)
{
   if (i < 0) {
      Error("SetCoeff: negative index");
      return;
   }

   long n, j;

   n = x.xrep.length();
   long wi = i/NTL_BITS_PER_LONG;

   if (wi >= n) {
      x.xrep.SetLength(wi+1);
      for (j = n; j <= wi; j++)
         x.xrep[j] = 0;
   }

   long bi = i - wi*NTL_BITS_PER_LONG;

   x.xrep[wi] |= (1UL << bi);
}
   


void SetCoeff(GF2X& x, long i, long val)
{
   if (i < 0) {
      Error("SetCoeff: negative index");
      return;
   }

   val = val & 1;

   if (val) {
      SetCoeff(x, i);
      return;
   }

   // we want to clear position i

   long n;

   n = x.xrep.length();
   long wi = i/NTL_BITS_PER_LONG;

   if (wi >= n) 
      return;

   long bi = i - wi*NTL_BITS_PER_LONG;

   x.xrep[wi] &= ~(1UL << bi);
   if (wi == n-1) x.normalize();
}

void SetCoeff(GF2X& x, long i, GF2 a)
{
   SetCoeff(x, i, rep(a));
}


void swap(GF2X& a, GF2X& b)
{
   swap(a.xrep, b.xrep);
}



long deg(const GF2X& aa)
{
   long n = aa.xrep.length();

   if (n == 0)
      return -1;

   _ntl_ulong a = aa.xrep[n-1];
   long i = 0;

   if (a == 0) Error("GF2X: unnormalized polynomial detected in deg");

   while (a>=256)
      i += 8, a >>= 8;
   if (a >=16)
      i += 4, a >>= 4;
   if (a >= 4)
      i += 2, a >>= 2;
   if (a >= 2)
      i += 2;
   else if (a >= 1)
      i++;

   return NTL_BITS_PER_LONG*(n-1) + i - 1;
}
   

long operator==(const GF2X& a, const GF2X& b)
{
   return a.xrep == b.xrep;
}

long operator==(const GF2X& a, long b)
{
   if (b & 1) 
      return IsOne(a);
   else
      return IsZero(a);
}

long operator==(const GF2X& a, GF2 b)
{
   if (b == 1) 
      return IsOne(a);
   else
      return IsZero(a);
}

void random(GF2X& x, long n)
{
   if (n < 0) Error("GF2X random: negative length");

   if (NTL_OVERFLOW(n, 1, 0))
      Error("GF2X random: excessive length");

   long wl = (n+NTL_BITS_PER_LONG-1)/NTL_BITS_PER_LONG;

   x.xrep.SetLength(wl);

   long i;
   for (i = 0; i < wl-1; i++) {
      x.xrep[i] = RandomWord();
   }

   if (n > 0) {
      long pos = n % NTL_BITS_PER_LONG;
      if (pos == 0) pos = NTL_BITS_PER_LONG;
      x.xrep[wl-1] = RandomBits_ulong(pos);
   }

   x.normalize();
}

void add(GF2X& x, const GF2X& a, const GF2X& b)
{
   long sa = a.xrep.length();
   long sb = b.xrep.length();

   long i;

   if (sa == sb) {
      x.xrep.SetLength(sa);
      if (sa == 0) return;

      _ntl_ulong *xp = x.xrep.elts();
      const _ntl_ulong *ap = a.xrep.elts();
      const _ntl_ulong *bp = b.xrep.elts();

      for (i = 0; i < sa; i++)
         xp[i] = ap[i] ^ bp[i];

      i = sa-1;
      while (i >= 0 && !xp[i]) i--;
      x.xrep.QuickSetLength(i+1);
   }
   
   else if (sa < sb) {
      x.xrep.SetLength(sb);
      _ntl_ulong *xp = x.xrep.elts();
      const _ntl_ulong *ap = a.xrep.elts();
      const _ntl_ulong *bp = b.xrep.elts();

      for (i = 0; i < sa; i++)
         xp[i] = ap[i] ^ bp[i];

      for (; i < sb; i++)
         xp[i] = bp[i];
   }
   else { // sa > sb
      x.xrep.SetLength(sa);
      _ntl_ulong *xp = x.xrep.elts();
      const _ntl_ulong *ap = a.xrep.elts();
      const _ntl_ulong *bp = b.xrep.elts();

      for (i = 0; i < sb; i++)
         xp[i] = ap[i] ^ bp[i];

      for (; i < sa; i++)
         xp[i] = ap[i];
   }
}






// This mul1 routine (for 32 x 32 bit multiplies) 
// was arrived at after a good deal of experimentation.
// Several people have advocated that the "best" way
// is to reduce it via karastuba to 9 8x8 multiplies, implementing
// the latter via table look-up (a huge table).  Although such a mul1
// would indeed have fewer machine instructions, my
// experiments on a PowerPC and on a Pentium indicate
// that the memory accesses slow it down.  On these machines
// the following mul1 is faster by a factor of about 1.5.


// inlining mul1 seems to help with g++; morever,
// the IBM xlC compiler inlines it anyway.

static inline
void mul1(_ntl_ulong *c, _ntl_ulong a, _ntl_ulong b)
{
   _ntl_ulong hi, lo;
   _ntl_ulong A[4];

   A[0] = 0;
   A[1] = a & ((1UL << (NTL_BITS_PER_LONG-1))-1UL);
   A[2] = a << 1;
   A[3] = A[1] ^ A[2];

   lo = A[b>>(NTL_BITS_PER_LONG-2)];
   hi = lo >> (NTL_BITS_PER_LONG-2); 
   lo = (lo << 2) ^ A[(b >> (NTL_BITS_PER_LONG-4)) & 3];

   // The following code is included from mach_desc.h
   // and handles *any* word size

   NTL_BB_MUL_CODE

   hi = (hi << 2)|(lo >> (NTL_BITS_PER_LONG-2)); 
   lo = (lo << 2) ^ A[b & 3];

   if (a >> (NTL_BITS_PER_LONG-1)) {
      hi = hi ^ (b >> 1);
      lo = lo ^ (b << (NTL_BITS_PER_LONG-1));
   }

   c[0] = lo;
   c[1] = hi;
}

static inline
void mul_half(_ntl_ulong *c, _ntl_ulong a, _ntl_ulong b)
{
   _ntl_ulong hi, lo;
   _ntl_ulong A[4];

   A[0] = 0;
   A[1] = a & ((1UL << (NTL_BITS_PER_LONG-1))-1UL);
   A[2] = a << 1;
   A[3] = A[1] ^ A[2];

   lo = A[b>>(NTL_BITS_PER_LONG/2-2)];
   hi = lo >> (NTL_BITS_PER_LONG-2); 
   lo = (lo << 2) ^ A[(b >> (NTL_BITS_PER_LONG/2-4)) & 3];


   NTL_BB_HALF_MUL_CODE


   hi = (hi << 2)|(lo >> (NTL_BITS_PER_LONG-2)); 
   lo = (lo << 2) ^ A[b & 3];

   if (a >> (NTL_BITS_PER_LONG-1)) {
      hi = hi ^ (b >> 1);
      lo = lo ^ (b << (NTL_BITS_PER_LONG-1));
   }

   c[0] = lo;
   c[1] = hi;
}


// mul2...mul8 hard-code 2x2...8x8 word multiplies.
// I adapted these routines from LiDIA.
// Inlining these seems to hurt, not help.

static
void mul2(_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b)
{
   _ntl_ulong hs0, hs1;
   _ntl_ulong hl2[2];

   hs0 = a[0] ^ a[1];
   hs1 = b[0] ^ b[1];

   mul1(c, a[0], b[0]);
   mul1(c+2, a[1], b[1]);
   mul1(hl2, hs0, hs1);


   hl2[0] = hl2[0] ^ c[0] ^ c[2];
   hl2[1] = hl2[1] ^ c[1] ^ c[3];

   c[1] ^= hl2[0];
   c[2] ^= hl2[1];
}

static
void mul3 (_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b)
{
   _ntl_ulong hs0[2], hs1[2];
   _ntl_ulong hl2[4];

   hs0[0] = a[0] ^ a[2]; 
   hs0[1] = a[1];
   hs1[0] = b[0] ^ b[2]; 
   hs1[1] = b[1];

   mul2(c, a, b); 
   mul1(c+4, a[2], b[2]);
   mul2(hl2, hs0, hs1); 

   hl2[0] = hl2[0] ^ c[0] ^ c[4]; 
   hl2[1] = hl2[1] ^ c[1] ^ c[5];
   hl2[2] = hl2[2] ^ c[2];
   hl2[3] = hl2[3] ^ c[3];
   
   c[2] ^= hl2[0];
   c[3] ^= hl2[1];
   c[4] ^= hl2[2];
   c[5] ^= hl2[3];
}

static
void mul4(_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b)
{
   _ntl_ulong hs0[2], hs1[2];
   _ntl_ulong hl2[4];

   hs0[0] = a[0] ^ a[2];
   hs0[1] = a[1] ^ a[3];
   hs1[0] = b[0] ^ b[2];
   hs1[1] = b[1] ^ b[3];

   mul2(c, a, b);
   mul2(c+4, a+2, b+2); 
   mul2(hl2, hs0, hs1);

   hl2[0] = hl2[0] ^ c[0] ^ c[4];
   hl2[1] = hl2[1] ^ c[1] ^ c[5];
   hl2[2] = hl2[2] ^ c[2] ^ c[6];
   hl2[3] = hl2[3] ^ c[3] ^ c[7];
   
   c[2] ^= hl2[0];
   c[3] ^= hl2[1];
   c[4] ^= hl2[2];
   c[5] ^= hl2[3];
}

static
void mul5 (_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b)
{
   _ntl_ulong hs0[3], hs1[3];
   _ntl_ulong hl2[6];

   hs0[0] = a[0] ^ a[3]; 
   hs0[1] = a[1] ^ a[4];
   hs0[2] = a[2];
   hs1[0] = b[0] ^ b[3]; 
   hs1[1] = b[1] ^ b[4];
   hs1[2] = b[2];

   mul3(c, a, b); 
   mul2(c+6, a+3, b+3); 
   mul3(hl2, hs0, hs1);

   hl2[0] = hl2[0] ^ c[0] ^ c[6]; 
   hl2[1] = hl2[1] ^ c[1] ^ c[7];
   hl2[2] = hl2[2] ^ c[2] ^ c[8];
   hl2[3] = hl2[3] ^ c[3] ^ c[9];
   hl2[4] = hl2[4] ^ c[4];
   hl2[5] = hl2[5] ^ c[5];

  
   c[3] ^= hl2[0];
   c[4] ^= hl2[1];
   c[5] ^= hl2[2];
   c[6] ^= hl2[3];
   c[7] ^= hl2[4];
   c[8] ^= hl2[5];
}

static
void mul6(_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b)
{
   _ntl_ulong hs0[3], hs1[3];
   _ntl_ulong hl2[6];

   hs0[0] = a[0] ^ a[3];   
   hs0[1] = a[1] ^ a[4];
   hs0[2] = a[2] ^ a[5];
   hs1[0] = b[0] ^ b[3];   
   hs1[1] = b[1] ^ b[4];
   hs1[2] = b[2] ^ b[5];

   mul3(c, a, b);   
   mul3(c+6, a+3, b+3); 
   mul3(hl2, hs0, hs1);  

   hl2[0] = hl2[0] ^ c[0] ^ c[6];
   hl2[1] = hl2[1] ^ c[1] ^ c[7];
   hl2[2] = hl2[2] ^ c[2] ^ c[8];
   hl2[3] = hl2[3] ^ c[3] ^ c[9];
   hl2[4] = hl2[4] ^ c[4] ^ c[10];
   hl2[5] = hl2[5] ^ c[5] ^ c[11];
   
   c[3] ^= hl2[0];
   c[4] ^= hl2[1];
   c[5] ^= hl2[2];
   c[6] ^= hl2[3];
   c[7] ^= hl2[4];
   c[8] ^= hl2[5];
}

static
void mul7(_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b)
{
   _ntl_ulong hs0[4], hs1[4];
   _ntl_ulong hl2[8];

   hs0[0] = a[0] ^ a[4]; 
   hs0[1] = a[1] ^ a[5];
   hs0[2] = a[2] ^ a[6];
   hs0[3] = a[3];
   hs1[0] = b[0] ^ b[4]; 
   hs1[1] = b[1] ^ b[5];
   hs1[2] = b[2] ^ b[6];
   hs1[3] = b[3];

   mul4(c, a, b); 
   mul3(c+8, a+4, b+4); 
   mul4(hl2, hs0, hs1);

   hl2[0] = hl2[0] ^ c[0] ^ c[8];
   hl2[1] = hl2[1] ^ c[1] ^ c[9];
   hl2[2] = hl2[2] ^ c[2] ^ c[10];
   hl2[3] = hl2[3] ^ c[3] ^ c[11];
   hl2[4] = hl2[4] ^ c[4] ^ c[12];
   hl2[5] = hl2[5] ^ c[5] ^ c[13];
   hl2[6] = hl2[6] ^ c[6];
   hl2[7] = hl2[7] ^ c[7];
   
   c[4]  ^= hl2[0];
   c[5]  ^= hl2[1];
   c[6]  ^= hl2[2];
   c[7]  ^= hl2[3];
   c[8]  ^= hl2[4];
   c[9]  ^= hl2[5];
   c[10] ^= hl2[6];
   c[11] ^= hl2[7];
}

static
void mul8(_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b)
{
   _ntl_ulong hs0[4], hs1[4];
   _ntl_ulong hl2[8];

   hs0[0] = a[0] ^ a[4]; 
   hs0[1] = a[1] ^ a[5];
   hs0[2] = a[2] ^ a[6];
   hs0[3] = a[3] ^ a[7];
   hs1[0] = b[0] ^ b[4]; 
   hs1[1] = b[1] ^ b[5];
   hs1[2] = b[2] ^ b[6];
   hs1[3] = b[3] ^ b[7];

   mul4(c, a, b); 
   mul4(c+8, a+4, b+4);
   mul4(hl2, hs0, hs1); 

   hl2[0] = hl2[0] ^ c[0] ^ c[8];
   hl2[1] = hl2[1] ^ c[1] ^ c[9];
   hl2[2] = hl2[2] ^ c[2] ^ c[10];
   hl2[3] = hl2[3] ^ c[3] ^ c[11];
   hl2[4] = hl2[4] ^ c[4] ^ c[12];
   hl2[5] = hl2[5] ^ c[5] ^ c[13];
   hl2[6] = hl2[6] ^ c[6] ^ c[14];
   hl2[7] = hl2[7] ^ c[7] ^ c[15];
   
   c[4]  ^= hl2[0];
   c[5]  ^= hl2[1];
   c[6]  ^= hl2[2];
   c[7]  ^= hl2[3];
   c[8]  ^= hl2[4];
   c[9]  ^= hl2[5];
   c[10] ^= hl2[6];
   c[11] ^= hl2[7];
}

static
void KarMul(_ntl_ulong *c, const _ntl_ulong *a, const _ntl_ulong *b, 
            long len, _ntl_ulong *stk)
{
   if (len <= 8) {
      switch (len) {
         case 1: mul1(c, a[0], b[0]); break;
         case 2: mul2(c, a, b); break;
         case 3: mul3(c, a, b); break;
         case 4: mul4(c, a, b); break;
         case 5: mul5(c, a, b); break;
         case 6: mul6(c, a, b); break;
         case 7: mul7(c, a, b); break;
         case 8: mul8(c, a, b); break;
      }

      return;
   }

   long ll, lh, i, ll2, lh2;
   const _ntl_ulong *a0, *a1, *b0, *b1;
   _ntl_ulong *a01, *b01, *h;

   lh = len >> 1;
   ll = (len+1) >> 1;

   ll2 = ll << 1;
   lh2 = lh << 1;

   a01 = stk;  stk += ll+1;
   b01 = stk;  stk += ll+1;
   h = stk; stk += ll2+1;

   a0 = a;
   a1 = a+ll;
   b0 = b;
   b1 = b+ll;

   KarMul(c, a0, b0, ll, stk);
   KarMul(c+ll2, a1, b1, lh, stk);

   for (i = 0; i < lh; i++) {
      a01[i] = a[i] ^ a[i+ll];
      b01[i] = b[i] ^ b[i+ll];
   }

   if (lh < ll) {
      a01[lh] = a[lh];
      b01[lh] = b[lh];
   }

   KarMul(h, a01, b01, ll, stk);

   for (i = 0; i < ll2; i++)
      h[i] ^= c[i];

   for (i = 0; i < lh2; i++)
      h[i] ^= c[i+ll2];

   for (i = 0; i < ll2; i++)
      c[i+ll] ^= h[i];
}


void mul(GF2X& c, const GF2X& a, const GF2X& b)
{
   long sa = a.xrep.length();
   long sb = b.xrep.length();

   if (sa <= 0 || sb <= 0) {
      clear(c);
      return;
   }
 
   _ntl_ulong a0 = a.xrep[0];
   _ntl_ulong b0 = b.xrep[0];

   if (sb == 1 && b0 == 1) {
      c = a;
      return;
   }

   if (sa == 1 && a0 == 1) {
      c = b;
      return;
   }

   if (&a == &b) {
      sqr(c, a);
      return;
   }

   if (sa == sb && sa <= 8) {
      // we treat these cases specially for efficiency reasons

      switch (sa) {
         case 1: {
            _ntl_ulong v[2];
            if (!(a0 >> NTL_BITS_PER_LONG/2))
               mul_half(v, b0, a0);
            else if (!(b0 >> NTL_BITS_PER_LONG/2))
               mul_half(v, a0, b0);
            else
               mul1(v, a0, b0);

            if (v[1]) {
               c.xrep.SetLength(2);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
            }
            else {
               c.xrep.SetLength(1);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
            }
         }
         return;

         case 2: {
            _ntl_ulong v[4];
            mul2(v, &a.xrep[0], &b.xrep[0]);
            if (v[3]) {
               c.xrep.SetLength(4);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
            }
            else {
               c.xrep.SetLength(3);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
            }
         }
         return;

         case 3: {
            _ntl_ulong v[6];
            mul3(v, &a.xrep[0], &b.xrep[0]);
            if (v[5]) {
               c.xrep.SetLength(6);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
            }
            else {
               c.xrep.SetLength(5);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
            }
         }
         return;

         case 4: {
            _ntl_ulong v[8];
            mul4(v, &a.xrep[0], &b.xrep[0]);
            if (v[7]) {
               c.xrep.SetLength(8);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
            }
            else {
               c.xrep.SetLength(7);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
            }
         }
         return;

         case 5: {
            _ntl_ulong v[10];
            mul5(v, &a.xrep[0], &b.xrep[0]);
            if (v[9]) {
               c.xrep.SetLength(10);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
               cp[9] = v[9];
            }
            else {
               c.xrep.SetLength(9);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
            }
         }
         return;

         case 6: {
            _ntl_ulong v[12];
            mul6(v, &a.xrep[0], &b.xrep[0]);
            if (v[11]) {
               c.xrep.SetLength(12);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
               cp[9] = v[9];
               cp[10] = v[10];
               cp[11] = v[11];
            }
            else {
               c.xrep.SetLength(11);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
               cp[9] = v[9];
               cp[10] = v[10];
            }
         }
         return; 

         case 7: {
            _ntl_ulong v[14];
            mul7(v, &a.xrep[0], &b.xrep[0]);
            if (v[13]) {
               c.xrep.SetLength(14);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
               cp[9] = v[9];
               cp[10] = v[10];
               cp[11] = v[11];
               cp[12] = v[12];
               cp[13] = v[13];
            }
            else {
               c.xrep.SetLength(13);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
               cp[9] = v[9];
               cp[10] = v[10];
               cp[11] = v[11];
               cp[12] = v[12];
            }
         }
         return; 

         case 8: {
            _ntl_ulong v[16];
            mul8(v, &a.xrep[0], &b.xrep[0]);
            if (v[15]) {
               c.xrep.SetLength(16);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
               cp[9] = v[9];
               cp[10] = v[10];
               cp[11] = v[11];
               cp[12] = v[12];
               cp[13] = v[13];
               cp[14] = v[14];
               cp[15] = v[15];
            }
            else {
               c.xrep.SetLength(15);
               _ntl_ulong *cp = &c.xrep[0];
               cp[0] = v[0];
               cp[1] = v[1];
               cp[2] = v[2];
               cp[3] = v[3];
               cp[4] = v[4];
               cp[5] = v[5];
               cp[6] = v[6];
               cp[7] = v[7];
               cp[8] = v[8];
               cp[9] = v[9];
               cp[10] = v[10];
               cp[11] = v[11];
               cp[12] = v[12];
               cp[13] = v[13];
               cp[14] = v[14];
            }
         }
         return; 

      }
   }

   // another special case:  one of the two inputs
   // has length 1 (and maybe 1/2).

   if (sa == 1) {
      c.xrep.SetLength(sb + 1);
      _ntl_ulong *cp = c.xrep.elts();
      const _ntl_ulong *bp = b.xrep.elts();

      long i;
      _ntl_ulong carry;
      _ntl_ulong v[2];

      carry = 0;

      if (a0 >> NTL_BITS_PER_LONG/2) {
         for (i = 0; i < sb; i++) {
            mul1(v, bp[i], a0);
            cp[i] = carry ^ v[0];
            carry = v[1];
         }
      }
      else {
         for (i = 0; i < sb; i++) {
            mul_half(v, bp[i], a0);
            cp[i] = carry ^ v[0];
            carry = v[1];
         }
      }

      cp[sb] = carry;
      c.normalize();
      return;
   }

   if (sb == 1) {
      c.xrep.SetLength(sa + 1);
      _ntl_ulong *cp = c.xrep.elts();
      const _ntl_ulong *ap = a.xrep.elts();

      long i;
      _ntl_ulong carry;
      _ntl_ulong v[2];

      carry = 0;

      if (b0 >> NTL_BITS_PER_LONG/2) {
         for (i = 0; i < sa; i++) {
            mul1(v, ap[i], b0);
            cp[i] = carry ^ v[0];
            carry = v[1];
         }
      }
      else {
         for (i = 0; i < sa; i++) {
            mul_half(v, ap[i], b0);
            cp[i] = carry ^ v[0];
            carry = v[1];
         }
      }

      cp[sa] = carry;
      c.normalize();
      return;
   }

   // finally: the general case

   
   static WordVector mem;
   static WordVector stk;
   static WordVector vec;

   const _ntl_ulong *ap, *bp;
   _ntl_ulong *cp;


   long sc = sa + sb;
   long in_mem = 0;

   if (&a == &c || &b == &c) {
      mem.SetLength(sc);
      cp = mem.elts();
      in_mem = 1;
   }
   else {
      c.xrep.SetLength(sc);
      cp = c.xrep.elts();
   }


   long n, hn, sp;

   n = min(sa, sb);
   sp = 0;
   while (n > 8) {
      hn = (n+1) >> 1;
      sp += (hn << 2) + 3;
      n = hn;
   }

   stk.SetLength(sp);
   _ntl_ulong *stk_p = stk.elts();

   if (sa > sb) {
      { long t; t = sa; sa = sb; sb = t; }
      ap = b.xrep.elts();
      bp = a.xrep.elts();
   }
   else {
      ap = a.xrep.elts();
      bp = b.xrep.elts();
   }


   vec.SetLength(2*sa);

   _ntl_ulong *v = vec.elts();

   long i, j;

   for (i = 0; i < sc; i++)
      cp[i] = 0;

   do {
      if (sa == 0) break;

      if (sa == 1) {
         for (i = 0; i < sb; i++) {
            mul1(v, ap[0], bp[i]);
            cp[i] ^= v[0];
            cp[i+1] ^= v[1];
         }

         break;
      }

      // general case

      for (i = 0; i+sa <= sb; i += sa) {
         KarMul(v, ap, bp + i, sa, stk_p);
         for (j = 0; j < 2*sa; j++)
            cp[i+j] ^= v[j]; 
      }

      { const _ntl_ulong *t; t = ap; ap = bp + i; bp = t; }
      { long t; t = sa; sa = sb - i; sb = t; }
      cp = cp + i;
   } while (1);

   if (in_mem)
      c.xrep = mem;

   c.normalize();
}



void mul(GF2X& c, const GF2X& a, long b)
{
   if (b & 1)
      c = a;
   else
      clear(c);
}

void mul(GF2X& c, const GF2X& a, GF2 b)
{
   if (b == 1)
      c = a;
   else
      clear(c);
}


void trunc(GF2X& x, const GF2X& a, long m)
{
   if (m < 0) Error("trunc: bad args");

   long n = a.xrep.length();
   if (n == 0 || m == 0) {
      clear(x);
      return;
   }

   if (&x == &a) {
      if (n*NTL_BITS_PER_LONG > m) {
         long wm = (m-1)/NTL_BITS_PER_LONG;
         long bm = m - NTL_BITS_PER_LONG*wm;
         _ntl_ulong msk;
         if (bm == NTL_BITS_PER_LONG)
            msk = ~(0UL);
         else
            msk = ((1UL << bm) - 1UL);
         x.xrep[wm] &= msk;
         x.xrep.QuickSetLength(wm+1);
         x.normalize();
      }
   }
   else if (n*NTL_BITS_PER_LONG <= m) 
      x = a;
   else {
      long wm = (m-1)/NTL_BITS_PER_LONG;
      long bm = m - NTL_BITS_PER_LONG*wm;
      x.xrep.SetLength(wm+1);
      _ntl_ulong *xp = &x.xrep[0];
      const _ntl_ulong *ap = &a.xrep[0];
      long i;
      for (i = 0; i < wm; i++)
         xp[i] = ap[i];
      _ntl_ulong msk;
      if (bm == NTL_BITS_PER_LONG)
         msk = ~(0UL);
      else
         msk = ((1UL << bm) - 1UL);
      xp[wm] = ap[wm] & msk;
      x.normalize();
   }
}
      

/****** implementation of vec_GF2X ******/


NTL_vector_impl(GF2X,vec_GF2X)

NTL_eq_vector_impl(GF2X,vec_GF2X)



void MulByX(GF2X& x, const GF2X& a)
{
   long n = a.xrep.length();
   if (n == 0) {
      clear(x);
      return;
   }

   if (a.xrep[n-1] & (1UL << (NTL_BITS_PER_LONG-1))) {
      x.xrep.SetLength(n+1);
      x.xrep[n] = 1;
   }
   else if (&x != &a)
      x.xrep.SetLength(n);

   _ntl_ulong *xp = &x.xrep[0];
   const _ntl_ulong *ap = &a.xrep[0];

   long i;
   for (i = n-1; i > 0; i--)
      xp[i] = (ap[i] << 1) | (ap[i-1] >> (NTL_BITS_PER_LONG-1));

   xp[0] = ap[0] << 1;

   // no need to normalize
}



static _ntl_ulong sqrtab[256] = {

0UL, 1UL, 4UL, 5UL, 16UL, 17UL, 20UL, 21UL, 64UL, 
65UL, 68UL, 69UL, 80UL, 81UL, 84UL, 85UL, 256UL, 
257UL, 260UL, 261UL, 272UL, 273UL, 276UL, 277UL, 320UL, 
321UL, 324UL, 325UL, 336UL, 337UL, 340UL, 341UL, 1024UL, 
1025UL, 1028UL, 1029UL, 1040UL, 1041UL, 1044UL, 1045UL, 1088UL, 
1089UL, 1092UL, 1093UL, 1104UL, 1105UL, 1108UL, 1109UL, 1280UL, 
1281UL, 1284UL, 1285UL, 1296UL, 1297UL, 1300UL, 1301UL, 1344UL, 
1345UL, 1348UL, 1349UL, 1360UL, 1361UL, 1364UL, 1365UL, 4096UL, 
4097UL, 4100UL, 4101UL, 4112UL, 4113UL, 4116UL, 4117UL, 4160UL, 
4161UL, 4164UL, 4165UL, 4176UL, 4177UL, 4180UL, 4181UL, 4352UL, 
4353UL, 4356UL, 4357UL, 4368UL, 4369UL, 4372UL, 4373UL, 4416UL, 
4417UL, 4420UL, 4421UL, 4432UL, 4433UL, 4436UL, 4437UL, 5120UL, 
5121UL, 5124UL, 5125UL, 5136UL, 5137UL, 5140UL, 5141UL, 5184UL, 
5185UL, 5188UL, 5189UL, 5200UL, 5201UL, 5204UL, 5205UL, 5376UL, 
5377UL, 5380UL, 5381UL, 5392UL, 5393UL, 5396UL, 5397UL, 5440UL, 
5441UL, 5444UL, 5445UL, 5456UL, 5457UL, 5460UL, 5461UL, 16384UL, 
16385UL, 16388UL, 16389UL, 16400UL, 16401UL, 16404UL, 16405UL, 16448UL, 
16449UL, 16452UL, 16453UL, 16464UL, 16465UL, 16468UL, 16469UL, 16640UL, 
16641UL, 16644UL, 16645UL, 16656UL, 16657UL, 16660UL, 16661UL, 16704UL, 
16705UL, 16708UL, 16709UL, 16720UL, 16721UL, 16724UL, 16725UL, 17408UL, 
17409UL, 17412UL, 17413UL, 17424UL, 17425UL, 17428UL, 17429UL, 17472UL, 
17473UL, 17476UL, 17477UL, 17488UL, 17489UL, 17492UL, 17493UL, 17664UL, 
17665UL, 17668UL, 17669UL, 17680UL, 17681UL, 17684UL, 17685UL, 17728UL, 
17729UL, 17732UL, 17733UL, 17744UL, 17745UL, 17748UL, 17749UL, 20480UL, 
20481UL, 20484UL, 20485UL, 20496UL, 20497UL, 20500UL, 20501UL, 20544UL, 
20545UL, 20548UL, 20549UL, 20560UL, 20561UL, 20564UL, 20565UL, 20736UL, 
20737UL, 20740UL, 20741UL, 20752UL, 20753UL, 20756UL, 20757UL, 20800UL, 
20801UL, 20804UL, 20805UL, 20816UL, 20817UL, 20820UL, 20821UL, 21504UL, 
21505UL, 21508UL, 21509UL, 21520UL, 21521UL, 21524UL, 21525UL, 21568UL, 
21569UL, 21572UL, 21573UL, 21584UL, 21585UL, 21588UL, 21589UL, 21760UL, 
21761UL, 21764UL, 21765UL, 21776UL, 21777UL, 21780UL, 21781UL, 21824UL, 
21825UL, 21828UL, 21829UL, 21840UL, 21841UL, 21844UL, 21845UL };




static inline 
void sqr1(_ntl_ulong *c, _ntl_ulong a)
{
   _ntl_ulong hi, lo;

   NTL_BB_SQR_CODE

   c[0] = lo;
   c[1] = hi;
}




void sqr(GF2X& c, const GF2X& a)
{
   long sa = a.xrep.length();
   if (sa <= 0) {
      clear(c);
      return;
   }

   c.xrep.SetLength(sa << 1);
   _ntl_ulong *cp = c.xrep.elts();
   const _ntl_ulong *ap = a.xrep.elts();
   long i;

   for (i = sa-1; i >= 0; i--)
      sqr1(cp + (i << 1), ap[i]);

   c.normalize();
   return;
}



void LeftShift(GF2X& c, const GF2X& a, long n)
{
   if (IsZero(a)) {
      clear(c);
      return;
   }

   if (n == 1) {
      MulByX(c, a);
      return;
   }

   if (n < 0) {
      if (n < -NTL_MAX_LONG) 
         clear(c);
      else
         RightShift(c, a, -n);
      return;
   }

   if (NTL_OVERFLOW(n, 1, 0))
      Error("overflow in LeftShift");

   if (n == 0) {
      c = a;
      return;
   }

   long sa = a.xrep.length();

   long wn = n/NTL_BITS_PER_LONG;
   long bn = n - wn*NTL_BITS_PER_LONG;

   long sc = sa + wn;
   if (bn) sc++;

   c.xrep.SetLength(sc);

   _ntl_ulong *cp = c.xrep.elts();
   const _ntl_ulong *ap = a.xrep.elts();

   long i;

   if (bn == 0) {
      for (i = sa+wn-1; i >= wn; i--)
         cp[i] = ap[i-wn];
      for (i = wn-1; i >= 0; i--)
         cp[i] = 0;
   }
   else {
      cp[sa+wn] = ap[sa-1] >> (NTL_BITS_PER_LONG-bn);
      for (i = sa+wn-1; i >= wn+1; i--)
         cp[i] = (ap[i-wn] << bn) | (ap[i-wn-1] >> (NTL_BITS_PER_LONG-bn));
      cp[wn] = ap[0] << bn;
      for (i = wn-1; i >= 0; i--)
         cp[i] = 0;
   }

   c.normalize();
}

void ShiftAdd(GF2X& c, const GF2X& a, long n)
// c = c + a*X^n
{
   if (n < 0) Error("ShiftAdd: negative argument");

   if (n == 0) {
      add(c, c, a);
      return;
   }

   if (NTL_OVERFLOW(n, 1, 0))
      Error("overflow in ShiftAdd");

   long sa = a.xrep.length();
   if (sa <= 0) {
      return;
   }

   long sc = c.xrep.length();

   long wn = n/NTL_BITS_PER_LONG;
   long bn = n - wn*NTL_BITS_PER_LONG;

   long ss = sa + wn;
   if (bn) ss++;

   if (ss > sc) 
      c.xrep.SetLength(ss);

   _ntl_ulong *cp = c.xrep.elts();
   const _ntl_ulong *ap = a.xrep.elts();

   long i;

   for (i = sc; i < ss; i++)
      cp[i] = 0;

   if (bn == 0) {
      for (i = sa+wn-1; i >= wn; i--)
         cp[i] ^= ap[i-wn];
   }
   else {
      cp[sa+wn] ^= ap[sa-1] >> (NTL_BITS_PER_LONG-bn);
      for (i = sa+wn-1; i >= wn+1; i--)
         cp[i] ^= (ap[i-wn] << bn) | (ap[i-wn-1] >> (NTL_BITS_PER_LONG-bn));
      cp[wn] ^= ap[0] << bn;
   }

   c.normalize();
}




void RightShift(GF2X& c, const GF2X& a, long n)
{
   if (IsZero(a)) {
      clear(c);
      return;
   }

   if (n < 0) {
      if (n < -NTL_MAX_LONG) Error("overflow in RightShift");
      LeftShift(c, a, -n);
      return;
   }

   if (n == 0) {
      c = a;
      return;
   }

   long sa = a.xrep.length();
   long wn = n/NTL_BITS_PER_LONG;
   long bn = n - wn*NTL_BITS_PER_LONG;

   if (wn >= sa) {
      clear(c);
      return;
   }

   c.xrep.SetLength(sa-wn);
   _ntl_ulong *cp = c.xrep.elts();
   const _ntl_ulong *ap = a.xrep.elts();

   long i;

   if (bn == 0) {
      for (i = 0; i < sa-wn; i++)
         cp[i] = ap[i+wn];
   }
   else {
      for (i = 0; i < sa-wn-1; i++)
         cp[i] = (ap[i+wn] >> bn) | (ap[i+wn+1] << (NTL_BITS_PER_LONG - bn));

      cp[sa-wn-1] = ap[sa-1] >> bn;
   }

   c.normalize();
}



static _ntl_ulong revtab[256] = {

0UL, 128UL, 64UL, 192UL, 32UL, 160UL, 96UL, 224UL, 16UL, 144UL, 
80UL, 208UL, 48UL, 176UL, 112UL, 240UL, 8UL, 136UL, 72UL, 200UL, 
40UL, 168UL, 104UL, 232UL, 24UL, 152UL, 88UL, 216UL, 56UL, 184UL, 
120UL, 248UL, 4UL, 132UL, 68UL, 196UL, 36UL, 164UL, 100UL, 228UL, 
20UL, 148UL, 84UL, 212UL, 52UL, 180UL, 116UL, 244UL, 12UL, 140UL, 
76UL, 204UL, 44UL, 172UL, 108UL, 236UL, 28UL, 156UL, 92UL, 220UL, 
60UL, 188UL, 124UL, 252UL, 2UL, 130UL, 66UL, 194UL, 34UL, 162UL, 
98UL, 226UL, 18UL, 146UL, 82UL, 210UL, 50UL, 178UL, 114UL, 242UL, 
10UL, 138UL, 74UL, 202UL, 42UL, 170UL, 106UL, 234UL, 26UL, 154UL, 
90UL, 218UL, 58UL, 186UL, 122UL, 250UL, 6UL, 134UL, 70UL, 198UL, 
38UL, 166UL, 102UL, 230UL, 22UL, 150UL, 86UL, 214UL, 54UL, 182UL, 
118UL, 246UL, 14UL, 142UL, 78UL, 206UL, 46UL, 174UL, 110UL, 238UL, 
30UL, 158UL, 94UL, 222UL, 62UL, 190UL, 126UL, 254UL, 1UL, 129UL, 
65UL, 193UL, 33UL, 161UL, 97UL, 225UL, 17UL, 145UL, 81UL, 209UL, 
49UL, 177UL, 113UL, 241UL, 9UL, 137UL, 73UL, 201UL, 41UL, 169UL, 
105UL, 233UL, 25UL, 153UL, 89UL, 217UL, 57UL, 185UL, 121UL, 249UL, 
5UL, 133UL, 69UL, 197UL, 37UL, 165UL, 101UL, 229UL, 21UL, 149UL, 
85UL, 213UL, 53UL, 181UL, 117UL, 245UL, 13UL, 141UL, 77UL, 205UL, 
45UL, 173UL, 109UL, 237UL, 29UL, 157UL, 93UL, 221UL, 61UL, 189UL, 
125UL, 253UL, 3UL, 131UL, 67UL, 195UL, 35UL, 163UL, 99UL, 227UL, 
19UL, 147UL, 83UL, 211UL, 51UL, 179UL, 115UL, 243UL, 11UL, 139UL, 
75UL, 203UL, 43UL, 171UL, 107UL, 235UL, 27UL, 155UL, 91UL, 219UL, 
59UL, 187UL, 123UL, 251UL, 7UL, 135UL, 71UL, 199UL, 39UL, 167UL, 
103UL, 231UL, 23UL, 151UL, 87UL, 215UL, 55UL, 183UL, 119UL, 247UL, 
15UL, 143UL, 79UL, 207UL, 47UL, 175UL, 111UL, 239UL, 31UL, 159UL, 
95UL, 223UL, 63UL, 191UL, 127UL, 255UL  }; 

static inline 
_ntl_ulong rev1(_ntl_ulong a)
{
   return NTL_BB_REV_CODE;
}



void CopyReverse(GF2X& c, const GF2X& a, long hi)
// c[0..hi] = reverse(a[0..hi]), with zero fill as necessary
// input may alias output

{
   if (hi < 0) { clear(c); return; }

   if (NTL_OVERFLOW(hi, 1, 0))
      Error("overflow in CopyReverse");

   long n = hi+1;
   long sa = a.xrep.length();
   if (n <= 0 || sa <= 0) {
      clear(c);
      return;
   }

   long wn = n/NTL_BITS_PER_LONG;
   long bn = n - wn*NTL_BITS_PER_LONG;

   if (bn != 0) {
      wn++;
      bn = NTL_BITS_PER_LONG - bn;
   }

   c.xrep.SetLength(wn);
  
   _ntl_ulong *cp = c.xrep.elts();
   const _ntl_ulong *ap = a.xrep.elts();

   long mm = min(sa, wn);
   long i;

   for (i = 0; i < mm; i++)
      cp[i] = ap[i];

   for (i = mm; i < wn; i++)
      cp[i] = 0;

   if (bn != 0) {
      for (i = wn-1; i >= 1; i--)
         cp[i] = (cp[i] << bn) | (cp[i-1] >> (NTL_BITS_PER_LONG-bn));
      cp[0] = cp[0] << bn;
   }

   for (i = 0; i < wn/2; i++) {
      _ntl_ulong t; t = cp[i]; cp[i] = cp[wn-1-i]; cp[wn-1-i] = t;
   }

   for (i = 0; i < wn; i++)
      cp[i] = rev1(cp[i]);

   c.normalize();
}


void div(GF2X& q, const GF2X& a, GF2 b)
{
   if (b == 0)
      Error("div: division by zero");

   q = a;
}

void div(GF2X& q, const GF2X& a, long b)
{
   if ((b & 1) == 0)
      Error("div: division by zero");

   q = a;
}



void GF2XFromBytes(GF2X& x, const unsigned char *p, long n)
{
   if (n <= 0) {
      x = 0;
      return;
   }

   const long BytesPerLong = NTL_BITS_PER_LONG/8;

   long lw, r, i, j;

   lw = n/BytesPerLong;
   r = n - lw*BytesPerLong;

   if (r != 0) 
      lw++;
   else
      r = BytesPerLong;

   x.xrep.SetLength(lw);
   unsigned long *xp = x.xrep.elts();

   for (i = 0; i < lw-1; i++) {
      unsigned long t = 0;
      for (j = 0; j < BytesPerLong; j++) {
         t >>= 8;
         t += (((unsigned long)(*p)) & 255UL) << ((BytesPerLong-1)*8);
         p++;
      }
      xp[i] = t;
   }

   unsigned long t = 0;
   for (j = 0; j < r; j++) {
      t >>= 8;
      t += (((unsigned long)(*p)) & 255UL) << ((BytesPerLong-1)*8);
      p++;
   }

   t >>= (BytesPerLong-r)*8;
   xp[lw-1] = t;

   x.normalize();
}

void BytesFromGF2X(unsigned char *p, const GF2X& a, long n)
{
   if (n < 0) n = 0;

   const long BytesPerLong = NTL_BITS_PER_LONG/8;

   long lbits = deg(a) + 1;
   long lbytes = (lbits+7)/8;

   long min_bytes = min(lbytes, n);

   long min_words = min_bytes/BytesPerLong;
   long r = min_bytes - min_words*BytesPerLong;
   if (r != 0)
      min_words++;
   else
      r = BytesPerLong;

   const unsigned long *ap = a.xrep.elts();

   long i, j;

   for (i = 0; i < min_words-1; i++) {
      unsigned long t = ap[i];
      for (j = 0; j < BytesPerLong; j++) {
         *p = t & 255UL;
         t >>= 8;
         p++;
      }
   }

   if (min_words > 0) {
      unsigned long t = ap[min_words-1];
      for (j = 0; j < r; j++) {
         *p = t & 255UL;
         t >>= 8;
         p++;
      }
   }

   for (j = min_bytes; j < n; j++) {
      *p = 0;
      p++;
   }
}

NTL_END_IMPL
