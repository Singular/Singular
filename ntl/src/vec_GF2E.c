

#include <NTL/vec_GF2E.h>

#include <NTL/new.h>

NTL_START_IMPL


const long MaxAllocBlock = 10000;




void BlockConstruct(GF2E* x, long n)
{
   if (n <= 0) return; 

   if (!GF2EInfo)
      Error("GF2E constructor called while modulus undefined");

   long d = GF2E::WordLength();

   long size = d + 2;
   long AllocAmt = (MaxAllocBlock-1) / size;
   if (AllocAmt == 0) AllocAmt = 1;

   long i = 0;
   long m;
   _ntl_ulong *p, *q;
   long j;

   while (i < n) {
      m = min((n-i), AllocAmt);
      p = (_ntl_ulong *) malloc((m*size + 1)*(sizeof (_ntl_ulong)));
      if (!p) Error("out of memory in BlockConstruct(GF2E*,long)");
      *p = m;
      for (j = 0, q = p+3; j < m; j++, i++, q += size) {
         q[-2] =  (d << 1) | 1;
         q[-1] = 0;
         x[i]._GF2E__rep.xrep.rep = q;
      }
   }
}

void BlockDestroy(GF2E* x, long n)
{
   if (n <= 0) return;

   long i = 0;
   _ntl_ulong *p;
   long m;

   while (i < n) {
      p = x[i]._GF2E__rep.xrep.rep-3;
      m =  *p;
      free(p);
      i += m;
   }
}


NTL_vector_impl_plain(GF2E,vec_GF2E)

NTL_eq_vector_impl(GF2E,vec_GF2E)


void InnerProduct(GF2E& x, const vec_GF2E& a, const vec_GF2E& b)
{
   long n = min(a.length(), b.length());
   long i;
   GF2X accum, t;

   clear(accum);
   for (i = 0; i < n; i++) {
      mul(t, rep(a[i]), rep(b[i]));
      add(accum, accum, t);
   }

   conv(x, accum);
}

void InnerProduct(GF2E& x, const vec_GF2E& a, const vec_GF2E& b,
                  long offset)
{
   long n = min(a.length(), b.length()+offset);
   long i;
   GF2X accum, t;

   clear(accum);
   for (i = offset; i < n; i++) {
      mul(t, rep(a[i]), rep(b[i-offset]));
      add(accum, accum, t);
   }

   conv(x, accum);
}

void mul(vec_GF2E& x, const vec_GF2E& a, const GF2E& b_in)
{
   GF2E b = b_in;
   long n = a.length();
   x.SetLength(n);
   long i;
   for (i = 0; i < n; i++)
      mul(x[i], a[i], b);
}

void mul(vec_GF2E& x, const vec_GF2E& a, GF2 b)
{
   x = a;
   if (b == 0)
      clear(x);
}


void add(vec_GF2E& x, const vec_GF2E& a, const vec_GF2E& b)
{
   long n = a.length();
   if (b.length() != n) Error("vector add: dimension mismatch");

   x.SetLength(n);
   long i;
   for (i = 0; i < n; i++)
      add(x[i], a[i], b[i]);
}


void clear(vec_GF2E& x)
{
   long n = x.length();
   long i;
   for (i = 0; i < n; i++)
      clear(x[i]);
}



long IsZero(const vec_GF2E& a)
{
   long n = a.length();
   long i;

   for (i = 0; i < n; i++)
      if (!IsZero(a[i]))
         return 0;

   return 1;
}

vec_GF2E operator+(const vec_GF2E& a, const vec_GF2E& b)
{
   vec_GF2E res;
   add(res, a, b);
   NTL_OPT_RETURN(vec_GF2E, res);
}

vec_GF2E operator-(const vec_GF2E& a, const vec_GF2E& b)
{
   vec_GF2E res;
   sub(res, a, b);
   NTL_OPT_RETURN(vec_GF2E, res);
}


vec_GF2E operator-(const vec_GF2E& a)
{
   vec_GF2E res;
   negate(res, a);
   NTL_OPT_RETURN(vec_GF2E, res);
}


GF2E operator*(const vec_GF2E& a, const vec_GF2E& b)
{
   GF2E res;
   InnerProduct(res, a, b);
   return res;
}


void VectorCopy(vec_GF2E& x, const vec_GF2E& a, long n)
{
   if (n < 0) Error("VectorCopy: negative length");
   if (n >= (1L << (NTL_BITS_PER_LONG-4))) Error("overflow in VectorCopy");

   long m = min(n, a.length());

   x.SetLength(n);

   long i;

   for (i = 0; i < m; i++)
      x[i] = a[i];

   for (i = m; i < n; i++)
      clear(x[i]);
}



NTL_END_IMPL
