

#include <stdio.h>

#include <NTL/version.h>


#if (defined(__GNUC__) && (defined(__i386__) || defined(__i486__) || defined(__i586__)))


#define AutoFix (1)

#else

#define AutoFix (0)

#endif


long f1(void);
long f2(long bpl);
double f3(void);
void f4(double *p);
void f5(int *p);
long f6(void);
long f7(void);
long f8(void);



double power2(long k)
{
   long i;
   double res;

   res = 1;

   for (i = 1; i <= k; i++)
      res = res * 2;

   return res;
}

long DoubleRounding(long dp)
{
   double a = power2(dp-1) + 1;
   double b = (power2(dp-6)-1)/power2(dp-5);
   double x;
   x = a + b;
   f4(&x);

   if (x != power2(dp-1) + 1)
      return 1;
   else 
      return 0; 
}


long DoublePrecision()
{
   long k;
   double l1 = (double)1;
   double lh = 1/(double)2;
   double epsilon;
   double fudge, oldfudge;

   epsilon = l1;
   fudge = l1+l1;

   k = 0;

   do {
      k++;
      epsilon = epsilon * lh;
      oldfudge = fudge;
      fudge = l1 + epsilon;
      f4(&fudge);
      f4(&oldfudge);
   } while (fudge > l1 && fudge < oldfudge);

   return k;
}

long DoublePrecision1()
{
   register double fudge, oldfudge;

   long k;
   double l1 = (double)1;
   double lh = 1/(double)2;
   double epsilon;


   epsilon = l1;
   fudge = l1+l1;

   k = 0;

   do {
      k++;
      epsilon = epsilon * lh;
      oldfudge = fudge;
      fudge = l1 + epsilon;
   } while (fudge > l1 && fudge < oldfudge);

   return k;
}

union d_or_rep {
   double d;
   unsigned long rep[2];
};

long RepTest(void)
{
   union d_or_rep v;

   v.rep[0] = v.rep[1] = 0;

   v.d = 565656565656.0;

   if (v.rep[0] == 0x42607678 && v.rep[1] == 0x46f30000)
      return 1;
   else if (v.rep[1] == 0x42607678 && v.rep[0] == 0x46f30000)
      return -1;
   else
      return 0;
}

void print2k(FILE *f, long k, long bpl)
{
   long m, l;
   long first;

   if (k <= 0) {
      fprintf(f, "((double) 1.0)");
      return;
   }

   m = bpl - 2;
   first = 1;

   fprintf(f, "(");

   while (k > 0) {
      if (k > m)
         l = m;
      else
         l = k;

      k = k - l;

      if (first)
         first = 0;
      else 
         fprintf(f, "*");

      fprintf(f, "((double)(1L<<%ld))", l);
   }

   fprintf(f, ")");
}


void print_BB_mul_code(FILE *f, long n)
{
   long i;

   fprintf(f, "\n\n");

   fprintf(f, "#define NTL_BB_MUL_CODE \\\n");

   for (i = n-6; i >= 2; i -= 2) {
      fprintf(f, "hi=(hi<<2)|(lo>>%ld); ", n-2);
      fprintf(f, "lo=(lo<<2)^A[(b>>%ld)&3];\\\n", i); 
   }

   fprintf(f, "\n\n");
}

void print_BB_half_mul_code(FILE *f, long n)
{
   long i;

   fprintf(f, "\n\n");

   fprintf(f, "#define NTL_BB_HALF_MUL_CODE \\\n");

   for (i = n/2-6; i >= 2; i -= 2) {
      fprintf(f, "hi=(hi<<2)|(lo>>%ld); ", n-2);
      fprintf(f, "lo=(lo<<2)^A[(b>>%ld)&3];\\\n", i); 
   }

   fprintf(f, "\n\n");
}


void print_BB_sqr_code(FILE *f, long n)
{
   long i, pos;

   fprintf(f, "\n\n");
   fprintf(f, "#define NTL_BB_SQR_CODE \\\n");
   fprintf(f, "lo=sqrtab[a&255];\\\n");
   pos = 16;

   for (i = 8; i < n; i += 8) {
      if (2*(i+8) <= n) {
         fprintf(f, "lo=lo|(sqrtab[(a>>%ld)&255]<<%ld);\\\n", i, pos);
         pos += 16;
      }
      else if (2*i == n) {
         fprintf(f, "hi=sqrtab[(a>>%ld)&255];\\\n", i);
         pos = 16;
      }
      else if (2*i > n) {
         fprintf(f, "hi=hi|(sqrtab[(a>>%ld)&255]<<%ld);\\\n", i, pos);
         pos += 16;
      }
      else { /* only applies if word size is not a multiple of 16 */
         fprintf(f, "_ntl_ulong t=sqrtab[(a>>%ld)&255];\\\n", i);
         fprintf(f, "lo=lo|(t<<%ld);\\\n", pos);
         fprintf(f, "hi=t>>%ld;\\\n", n-8);
         pos = 8;
      }
   }

   fprintf(f, "\n\n");
}


void print_BB_rev_code(FILE *f, long n)
{
   long i;

   fprintf(f, "\n\n");
   fprintf(f, "#define NTL_BB_REV_CODE ");

   for (i = 0; i < n; i += 8) {
      if (i != 0) fprintf(f, "\\\n|");
      fprintf(f, "(revtab[(a>>%ld)&255]<<%ld)", i, n-8-i);
   }

   fprintf(f, "\n\n");
}
   



char *yn_vec[2] = { "no", "yes" }; 



int main()
{
   long bpl, bpi, rs_arith, nbits, single_mul_ok;
   long a;
   long x;
   long dp, dp1, dr;
   FILE *f;
   int xx;
   long warnings = 0;
   int c;

   fprintf(stderr, "This is NTL version %s\n\n", NTL_VERSION);


   x = f1();
   if (~x != f8()) {
      fprintf(stderr, "BAD NEWS: machine must be 2's compliment.\n");
      return 1;
   }

   if ((x >> 1) == x)
      rs_arith = 1;
   else
      rs_arith = 0;

   bpl = 0;
   while (x != 0) {
      x = x << 1;
      bpl++;
   }

   bpi = 0;
   xx = 1;
   while (xx) {
      xx = xx << 1; 
      bpi++;
      f5(&xx);
   }

   a = f2(bpl);
   if (2*a != f7() || a*a != f6()) {
      fprintf(stderr, "BAD NEWS: machine must work modulo 2^wordsize.\n");
      return 1;
   }

   if (((long)f3()) != 1.0) {
      fprintf(stderr, "BAD NEWS: machine must truncate floating point.\n");
      return 1;
   }

   if (bpl & 7) {
      fprintf(stderr, "BAD NEWS: word size must be multiple of 8 bits.\n");
      return 1;
   }

   if (bpl < 32) {
      fprintf(stderr, "BAD NEWS: word size must be at least 32 bits.\n");
      return 1;
   }


   dp = DoublePrecision();
   dp1 = DoublePrecision1();
   dr = DoubleRounding(dp);

   if (bpl-2 < dp-3)
      nbits = bpl-2;
   else
      nbits = dp-3;

   if (nbits & 1) nbits--;

   if (nbits < 30) {
      fprintf(stderr, "BAD NEWS: NBITS too small.\n");
      return 1;
   }

   single_mul_ok = RepTest();

   fprintf(stderr, "GOOD NEWS: compatible machine.\n");
   fprintf(stderr, "summary of machine characteristics:\n");
   fprintf(stderr, "bits per long = %ld\n", bpl);
   fprintf(stderr, "bits per int = %ld\n", bpi);
   fprintf(stderr, "arith right shift = %s\n", yn_vec[rs_arith]);
   fprintf(stderr, "double precision = %ld\n", dp);
   fprintf(stderr, "NBITS (maximum) = %ld\n", nbits);
   fprintf(stderr, "single mul ok = %s\n", yn_vec[single_mul_ok != 0]);
   fprintf(stderr, "extended doubles = %s\n", yn_vec[dp1 > dp]);
   fprintf(stderr, "double rounding = %s\n", yn_vec[dr]);

   if (dp1 > dp && AutoFix)
      fprintf(stderr, "-- auto x86 fix\n");

   if (dp != 53) {
      warnings = 1;

      fprintf(stderr, "\n\nWARNING:\n\n"); 
      fprintf(stderr, "Nonstandard floating point precision.\n");
      fprintf(stderr, "IEEE standard is 53 bits.\n"); 
   }

#if (defined(__sparc__) && !defined(__sparc_v8__))

   warnings = 1;

   fprintf(stderr, "\n\nWARNING:\n\n");
   fprintf(stderr, "If this Sparc is a Sparc-10 or later (so it has\n");
   fprintf(stderr, "a hardware integer multiply instruction) you\n");
   fprintf(stderr, "should specify the -mv8 option in the makefile\n"); 
   fprintf(stderr, "to obtain more efficient code.\n");

#endif

   if (dp1 > dp && !AutoFix) {
      warnings = 1;
      fprintf(stderr, "\n\nWARNING:\n\n");
      fprintf(stderr, "This platform has extended double precision registers.\n");
      fprintf(stderr, "While that may sound like a good thing, it actually is not.\n");
      fprintf(stderr, "If this is a Pentium or other x86 and your compiler\n");
      fprintf(stderr, "is g++ or supports GNU 'asm' constructs, it is recommended\n");
      fprintf(stderr, "to compile NTL with the NTL_X86_FIX flag to get true IEEE floating point.\n");
      fprintf(stderr, "Set this flag by editing the file config.h.\n");
      fprintf(stderr, "The code should still work even if you don't set\n");
      fprintf(stderr, "this flag.  See quad_float.txt for details.\n\n");
   }

   if (dp1 <= dp && dr) {
      warnings = 1;
      fprintf(stderr, "\n\nWARNING:\n\n");
      fprintf(stderr, "Hmm....your machine double rounds, but the measured precision\n");
      fprintf(stderr, "does not reflect this.\n");
      fprintf(stderr, "Make sure you have optimizations turned on.\n\n");
   }

#if 0

   /* better not to be interactive */

   if (warnings) {
      fprintf(stderr, "Do you want to continue anyway[y/n]? ");
      c = getchar();
      if (c == 'n' || c == 'N') {
         fprintf(stderr, "Make the necessary changes to the makefile and/or config.h,\n");
         fprintf(stderr, "then type 'make clobber' and then 'make'.\n\n\n");
         return 1;
      }
   }

#endif

   f = fopen("mach_desc.h", "w");
   if (!f) {
      fprintf(stderr, "can't open mach_desc.h for writing\n");
      return 1;
   }

   fprintf(f, "#ifndef NTL_mach_desc__H\n");
   fprintf(f, "#define NTL_mach_desc__H\n\n\n");
   fprintf(f, "#define NTL_BITS_PER_LONG (%ld)\n", bpl);
   fprintf(f, "#define NTL_MAX_LONG (%ldL)\n", ((long) ((1UL<<(bpl-1))-1UL)));
   fprintf(f, "#define NTL_MAX_INT (%ld)\n", ((long) ((1UL<<(bpi-1))-1UL)));
   fprintf(f, "#define NTL_BITS_PER_INT (%ld)\n", bpi);
   fprintf(f, "#define NTL_ARITH_RIGHT_SHIFT (%ld)\n", rs_arith);
   fprintf(f, "#define NTL_NBITS_MAX (%ld)\n", nbits);
   fprintf(f, "#define NTL_DOUBLE_PRECISION (%ld)\n", dp);
   fprintf(f, "#define NTL_FDOUBLE_PRECISION ");
   print2k(f, dp-1, bpl);
   fprintf(f, "\n");
   fprintf(f, "#define NTL_QUAD_FLOAT_SPLIT (");
   print2k(f, dp - (dp/2), bpl);
   fprintf(f, "+1.0)\n");
   fprintf(f, "#define NTL_EXT_DOUBLE (%d)\n", dp1 > dp);
   fprintf(f, "#define NTL_SINGLE_MUL_OK (%d)\n", single_mul_ok != 0);
   fprintf(f, "#define NTL_DOUBLES_LOW_HIGH (%d)\n\n\n", single_mul_ok < 0);
   print_BB_mul_code(f, bpl);
   print_BB_half_mul_code(f, bpl);
   print_BB_sqr_code(f, bpl);
   print_BB_rev_code(f, bpl);

   fprintf(f, "#define NTL_MIN_LONG (-NTL_MAX_LONG - 1L)\n");
   fprintf(f, "#define NTL_MIN_INT  (-NTL_MAX_INT - 1)\n");

   fprintf(f, "#endif\n\n");

   fclose(f);

   fprintf(stderr, "\n\n");
   
   return 0;
}
