#ifndef NTL_mach_desc__H
#define NTL_mach_desc__H


#define NTL_BITS_PER_LONG (64)
#define NTL_MAX_LONG (9223372036854775807L)
#define NTL_MAX_INT (2147483647)
#define NTL_BITS_PER_INT (32)
#define NTL_BITS_PER_SIZE_T (64)
#define NTL_ARITH_RIGHT_SHIFT (1)
#define NTL_NBITS_MAX (50)
#define NTL_DOUBLE_PRECISION (53)
#define NTL_FDOUBLE_PRECISION (((double)(1L<<52)))
#define NTL_QUAD_FLOAT_SPLIT ((((double)(1L<<27)))+1.0)
#define NTL_EXT_DOUBLE (0)
#define NTL_SINGLE_MUL_OK (0)
#define NTL_DOUBLES_LOW_HIGH (0)




#define NTL_BB_MUL_CODE0 \
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
   t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
   t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
   t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
   t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
   t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
   t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
   t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
   t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
   t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
   t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
   t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
   t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
   t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
   t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
   if (a >> 63) hi ^= ((b & 0xeeeeeeeeeeeeeeeeUL) >> 1);\
   if ((a >> 62) & 1) hi ^= ((b & 0xccccccccccccccccUL) >> 2);\
   if ((a >> 61) & 1) hi ^= ((b & 0x8888888888888888UL) >> 3);\
   c[0] = lo;    c[1] = hi;\





#define NTL_BB_MUL_CODE1 \
   long i;\
   _ntl_ulong carry = 0, b;\
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      b = bp[i];\
      lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
      t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
      t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
      t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
      t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
      t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
      t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
      t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
      t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
      t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
      t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
      t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
      t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
      t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
      t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
      if (a >> 63) hi ^= ((b & 0xeeeeeeeeeeeeeeeeUL) >> 1);\
      if ((a >> 62) & 1) hi ^= ((b & 0xccccccccccccccccUL) >> 2);\
      if ((a >> 61) & 1) hi ^= ((b & 0x8888888888888888UL) >> 3);\
      cp[i] = carry ^ lo;    carry = hi;\
   }\
   cp[sb] = carry;\





#define NTL_BB_MUL_CODE2 \
   long i;\
   _ntl_ulong carry = 0, b;\
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      b = bp[i];\
      lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
      t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
      t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
      t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
      t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
      t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
      t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
      t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
      t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
      t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
      t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
      t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
      t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
      t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
      t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
      if (a >> 63) hi ^= ((b & 0xeeeeeeeeeeeeeeeeUL) >> 1);\
      if ((a >> 62) & 1) hi ^= ((b & 0xccccccccccccccccUL) >> 2);\
      if ((a >> 61) & 1) hi ^= ((b & 0x8888888888888888UL) >> 3);\
      cp[i] ^= (carry ^ lo);    carry = hi;\
   }\
   cp[sb] ^= carry;\





#define NTL_SHORT_BB_MUL_CODE1 \
   long i;\
   _ntl_ulong carry = 0, b;\
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      b = bp[i];\
      lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
      t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
      t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
      t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
      t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
      t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
      t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
      t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
      t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
      t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
      t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
      t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
      t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
      t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
      t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
      cp[i] = carry ^ lo;    carry = hi;\
   }\
   cp[sb] = carry;\





#define NTL_HALF_BB_MUL_CODE0 \
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[4];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   lo = A[b & 3]; t = A[(b >> 2) & 3]; hi = t >> 62; lo ^= t << 2;\
   t = A[(b >> 4) & 3]; hi ^= t >> 60; lo ^= t << 4;\
   t = A[(b >> 6) & 3]; hi ^= t >> 58; lo ^= t << 6;\
   t = A[(b >> 8) & 3]; hi ^= t >> 56; lo ^= t << 8;\
   t = A[(b >> 10) & 3]; hi ^= t >> 54; lo ^= t << 10;\
   t = A[(b >> 12) & 3]; hi ^= t >> 52; lo ^= t << 12;\
   t = A[(b >> 14) & 3]; hi ^= t >> 50; lo ^= t << 14;\
   t = A[(b >> 16) & 3]; hi ^= t >> 48; lo ^= t << 16;\
   t = A[(b >> 18) & 3]; hi ^= t >> 46; lo ^= t << 18;\
   t = A[(b >> 20) & 3]; hi ^= t >> 44; lo ^= t << 20;\
   t = A[(b >> 22) & 3]; hi ^= t >> 42; lo ^= t << 22;\
   t = A[(b >> 24) & 3]; hi ^= t >> 40; lo ^= t << 24;\
   t = A[(b >> 26) & 3]; hi ^= t >> 38; lo ^= t << 26;\
   t = A[(b >> 28) & 3]; hi ^= t >> 36; lo ^= t << 28;\
   t = A[b >> 30]; hi ^= t >> 34; lo ^= t << 30;\
   if (a >> 63) hi ^= ((b & 0xaaaaaaaaUL) >> 1);\
   c[0] = lo;    c[1] = hi;\





#define NTL_ALT_BB_MUL_CODE0 \
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   const _ntl_ulong t4 = A[(b >> 4) & 15]; \
   const _ntl_ulong t8 = A[(b >> 8) & 15]; \
   const _ntl_ulong t12 = A[(b >> 12) & 15]; \
   const _ntl_ulong t16 = A[(b >> 16) & 15]; \
   const _ntl_ulong t20 = A[(b >> 20) & 15]; \
   const _ntl_ulong t24 = A[(b >> 24) & 15]; \
   const _ntl_ulong t28 = A[(b >> 28) & 15]; \
   const _ntl_ulong t32 = A[(b >> 32) & 15]; \
   const _ntl_ulong t36 = A[(b >> 36) & 15]; \
   const _ntl_ulong t40 = A[(b >> 40) & 15]; \
   const _ntl_ulong t44 = A[(b >> 44) & 15]; \
   const _ntl_ulong t48 = A[(b >> 48) & 15]; \
   const _ntl_ulong t52 = A[(b >> 52) & 15]; \
   const _ntl_ulong t56 = A[(b >> 56) & 15]; \
   const _ntl_ulong t60 = A[b >> 60]; \
   const _ntl_ulong lo = A[b & 15] \
      ^ (t4 << 4)\
      ^ (t8 << 8)\
      ^ (t12 << 12)\
      ^ (t16 << 16)\
      ^ (t20 << 20)\
      ^ (t24 << 24)\
      ^ (t28 << 28)\
      ^ (t32 << 32)\
      ^ (t36 << 36)\
      ^ (t40 << 40)\
      ^ (t44 << 44)\
      ^ (t48 << 48)\
      ^ (t52 << 52)\
      ^ (t56 << 56)\
      ^ (t60 << 60);\
   const _ntl_ulong hi = (t4 >> 60)\
      ^ (t8 >> 56)\
      ^ (t12 >> 52)\
      ^ (t16 >> 48)\
      ^ (t20 >> 44)\
      ^ (t24 >> 40)\
      ^ (t28 >> 36)\
      ^ (t32 >> 32)\
      ^ (t36 >> 28)\
      ^ (t40 >> 24)\
      ^ (t44 >> 20)\
      ^ (t48 >> 16)\
      ^ (t52 >> 12)\
      ^ (t56 >> 8)\
      ^ (t60 >> 4)\
      ^ (((b & 0xeeeeeeeeeeeeeeeeUL) >> 1) & (-(a >> 63)))\
      ^ (((b & 0xccccccccccccccccUL) >> 2) & (-((a >> 62) & 1UL)))\
      ^ (((b & 0x8888888888888888UL) >> 3) & (-((a >> 61) & 1UL)));\
   c[0] = lo;    c[1] = hi;\





#define NTL_ALT_BB_MUL_CODE1 \
   long i;\
   _ntl_ulong carry = 0;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      const _ntl_ulong b = bp[i];\
      const _ntl_ulong t4 = A[(b >> 4) & 15]; \
      const _ntl_ulong t8 = A[(b >> 8) & 15]; \
      const _ntl_ulong t12 = A[(b >> 12) & 15]; \
      const _ntl_ulong t16 = A[(b >> 16) & 15]; \
      const _ntl_ulong t20 = A[(b >> 20) & 15]; \
      const _ntl_ulong t24 = A[(b >> 24) & 15]; \
      const _ntl_ulong t28 = A[(b >> 28) & 15]; \
      const _ntl_ulong t32 = A[(b >> 32) & 15]; \
      const _ntl_ulong t36 = A[(b >> 36) & 15]; \
      const _ntl_ulong t40 = A[(b >> 40) & 15]; \
      const _ntl_ulong t44 = A[(b >> 44) & 15]; \
      const _ntl_ulong t48 = A[(b >> 48) & 15]; \
      const _ntl_ulong t52 = A[(b >> 52) & 15]; \
      const _ntl_ulong t56 = A[(b >> 56) & 15]; \
      const _ntl_ulong t60 = A[b >> 60]; \
      const _ntl_ulong lo = A[b & 15] \
         ^ (t4 << 4)\
         ^ (t8 << 8)\
         ^ (t12 << 12)\
         ^ (t16 << 16)\
         ^ (t20 << 20)\
         ^ (t24 << 24)\
         ^ (t28 << 28)\
         ^ (t32 << 32)\
         ^ (t36 << 36)\
         ^ (t40 << 40)\
         ^ (t44 << 44)\
         ^ (t48 << 48)\
         ^ (t52 << 52)\
         ^ (t56 << 56)\
         ^ (t60 << 60);\
      const _ntl_ulong hi = (t4 >> 60)\
         ^ (t8 >> 56)\
         ^ (t12 >> 52)\
         ^ (t16 >> 48)\
         ^ (t20 >> 44)\
         ^ (t24 >> 40)\
         ^ (t28 >> 36)\
         ^ (t32 >> 32)\
         ^ (t36 >> 28)\
         ^ (t40 >> 24)\
         ^ (t44 >> 20)\
         ^ (t48 >> 16)\
         ^ (t52 >> 12)\
         ^ (t56 >> 8)\
         ^ (t60 >> 4)\
         ^ (((b & 0xeeeeeeeeeeeeeeeeUL) >> 1) & (-(a >> 63)))\
         ^ (((b & 0xccccccccccccccccUL) >> 2) & (-((a >> 62) & 1UL)))\
         ^ (((b & 0x8888888888888888UL) >> 3) & (-((a >> 61) & 1UL)));\
      cp[i] = carry ^ lo;    carry = hi;\
   }\
   cp[sb] = carry;\





#define NTL_ALT_BB_MUL_CODE2 \
   long i;\
   _ntl_ulong carry = 0;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      const _ntl_ulong b = bp[i];\
      const _ntl_ulong t4 = A[(b >> 4) & 15]; \
      const _ntl_ulong t8 = A[(b >> 8) & 15]; \
      const _ntl_ulong t12 = A[(b >> 12) & 15]; \
      const _ntl_ulong t16 = A[(b >> 16) & 15]; \
      const _ntl_ulong t20 = A[(b >> 20) & 15]; \
      const _ntl_ulong t24 = A[(b >> 24) & 15]; \
      const _ntl_ulong t28 = A[(b >> 28) & 15]; \
      const _ntl_ulong t32 = A[(b >> 32) & 15]; \
      const _ntl_ulong t36 = A[(b >> 36) & 15]; \
      const _ntl_ulong t40 = A[(b >> 40) & 15]; \
      const _ntl_ulong t44 = A[(b >> 44) & 15]; \
      const _ntl_ulong t48 = A[(b >> 48) & 15]; \
      const _ntl_ulong t52 = A[(b >> 52) & 15]; \
      const _ntl_ulong t56 = A[(b >> 56) & 15]; \
      const _ntl_ulong t60 = A[b >> 60]; \
      const _ntl_ulong lo = A[b & 15] \
         ^ (t4 << 4)\
         ^ (t8 << 8)\
         ^ (t12 << 12)\
         ^ (t16 << 16)\
         ^ (t20 << 20)\
         ^ (t24 << 24)\
         ^ (t28 << 28)\
         ^ (t32 << 32)\
         ^ (t36 << 36)\
         ^ (t40 << 40)\
         ^ (t44 << 44)\
         ^ (t48 << 48)\
         ^ (t52 << 52)\
         ^ (t56 << 56)\
         ^ (t60 << 60);\
      const _ntl_ulong hi = (t4 >> 60)\
         ^ (t8 >> 56)\
         ^ (t12 >> 52)\
         ^ (t16 >> 48)\
         ^ (t20 >> 44)\
         ^ (t24 >> 40)\
         ^ (t28 >> 36)\
         ^ (t32 >> 32)\
         ^ (t36 >> 28)\
         ^ (t40 >> 24)\
         ^ (t44 >> 20)\
         ^ (t48 >> 16)\
         ^ (t52 >> 12)\
         ^ (t56 >> 8)\
         ^ (t60 >> 4)\
         ^ (((b & 0xeeeeeeeeeeeeeeeeUL) >> 1) & (-(a >> 63)))\
         ^ (((b & 0xccccccccccccccccUL) >> 2) & (-((a >> 62) & 1UL)))\
         ^ (((b & 0x8888888888888888UL) >> 3) & (-((a >> 61) & 1UL)));\
      cp[i] ^= (carry ^ lo);    carry = hi;\
   }\
   cp[sb] ^= carry;\





#define NTL_ALT_SHORT_BB_MUL_CODE1 \
   long i;\
   _ntl_ulong carry = 0;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      const _ntl_ulong b = bp[i];\
      const _ntl_ulong t4 = A[(b >> 4) & 15]; \
      const _ntl_ulong t8 = A[(b >> 8) & 15]; \
      const _ntl_ulong t12 = A[(b >> 12) & 15]; \
      const _ntl_ulong t16 = A[(b >> 16) & 15]; \
      const _ntl_ulong t20 = A[(b >> 20) & 15]; \
      const _ntl_ulong t24 = A[(b >> 24) & 15]; \
      const _ntl_ulong t28 = A[(b >> 28) & 15]; \
      const _ntl_ulong t32 = A[(b >> 32) & 15]; \
      const _ntl_ulong t36 = A[(b >> 36) & 15]; \
      const _ntl_ulong t40 = A[(b >> 40) & 15]; \
      const _ntl_ulong t44 = A[(b >> 44) & 15]; \
      const _ntl_ulong t48 = A[(b >> 48) & 15]; \
      const _ntl_ulong t52 = A[(b >> 52) & 15]; \
      const _ntl_ulong t56 = A[(b >> 56) & 15]; \
      const _ntl_ulong t60 = A[b >> 60]; \
      const _ntl_ulong lo = A[b & 15] \
         ^ (t4 << 4)\
         ^ (t8 << 8)\
         ^ (t12 << 12)\
         ^ (t16 << 16)\
         ^ (t20 << 20)\
         ^ (t24 << 24)\
         ^ (t28 << 28)\
         ^ (t32 << 32)\
         ^ (t36 << 36)\
         ^ (t40 << 40)\
         ^ (t44 << 44)\
         ^ (t48 << 48)\
         ^ (t52 << 52)\
         ^ (t56 << 56)\
         ^ (t60 << 60);\
      const _ntl_ulong hi = (t4 >> 60)\
         ^ (t8 >> 56)\
         ^ (t12 >> 52)\
         ^ (t16 >> 48)\
         ^ (t20 >> 44)\
         ^ (t24 >> 40)\
         ^ (t28 >> 36)\
         ^ (t32 >> 32)\
         ^ (t36 >> 28)\
         ^ (t40 >> 24)\
         ^ (t44 >> 20)\
         ^ (t48 >> 16)\
         ^ (t52 >> 12)\
         ^ (t56 >> 8)\
         ^ (t60 >> 4);\
      cp[i] = carry ^ lo;    carry = hi;\
   }\
   cp[sb] = carry;\





#define NTL_ALT_HALF_BB_MUL_CODE0 \
   _ntl_ulong A[4];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   const _ntl_ulong t2 = A[(b >> 2) & 3]; \
   const _ntl_ulong t4 = A[(b >> 4) & 3]; \
   const _ntl_ulong t6 = A[(b >> 6) & 3]; \
   const _ntl_ulong t8 = A[(b >> 8) & 3]; \
   const _ntl_ulong t10 = A[(b >> 10) & 3]; \
   const _ntl_ulong t12 = A[(b >> 12) & 3]; \
   const _ntl_ulong t14 = A[(b >> 14) & 3]; \
   const _ntl_ulong t16 = A[(b >> 16) & 3]; \
   const _ntl_ulong t18 = A[(b >> 18) & 3]; \
   const _ntl_ulong t20 = A[(b >> 20) & 3]; \
   const _ntl_ulong t22 = A[(b >> 22) & 3]; \
   const _ntl_ulong t24 = A[(b >> 24) & 3]; \
   const _ntl_ulong t26 = A[(b >> 26) & 3]; \
   const _ntl_ulong t28 = A[(b >> 28) & 3]; \
   const _ntl_ulong t30 = A[b >> 30]; \
   const _ntl_ulong lo = A[b & 3] \
      ^ (t2 << 2)\
      ^ (t4 << 4)\
      ^ (t6 << 6)\
      ^ (t8 << 8)\
      ^ (t10 << 10)\
      ^ (t12 << 12)\
      ^ (t14 << 14)\
      ^ (t16 << 16)\
      ^ (t18 << 18)\
      ^ (t20 << 20)\
      ^ (t22 << 22)\
      ^ (t24 << 24)\
      ^ (t26 << 26)\
      ^ (t28 << 28)\
      ^ (t30 << 30);\
   const _ntl_ulong hi = (t2 >> 62)\
      ^ (t4 >> 60)\
      ^ (t6 >> 58)\
      ^ (t8 >> 56)\
      ^ (t10 >> 54)\
      ^ (t12 >> 52)\
      ^ (t14 >> 50)\
      ^ (t16 >> 48)\
      ^ (t18 >> 46)\
      ^ (t20 >> 44)\
      ^ (t22 >> 42)\
      ^ (t24 >> 40)\
      ^ (t26 >> 38)\
      ^ (t28 >> 36)\
      ^ (t30 >> 34)\
      ^ (((b & 0xaaaaaaaaUL) >> 1) & (-(a >> 63)));\
   c[0] = lo;    c[1] = hi;\





#define NTL_ALT1_BB_MUL_CODE0 \
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
   t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
   t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
   t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
   t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
   t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
   t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
   t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
   t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
   t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
   t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
   t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
   t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
   t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
   t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
   hi ^= (((b & 0xeeeeeeeeeeeeeeeeUL) >> 1) & (-(a >> 63)))\
      ^ (((b & 0xccccccccccccccccUL) >> 2) & (-((a >> 62) & 1UL)))\
      ^ (((b & 0x8888888888888888UL) >> 3) & (-((a >> 61) & 1UL)));\
   c[0] = lo;    c[1] = hi;\





#define NTL_ALT1_BB_MUL_CODE1 \
   long i;\
   _ntl_ulong carry = 0, b;\
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      b = bp[i];\
      lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
      t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
      t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
      t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
      t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
      t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
      t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
      t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
      t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
      t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
      t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
      t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
      t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
      t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
      t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
      hi ^= (((b & 0xeeeeeeeeeeeeeeeeUL) >> 1) & (-(a >> 63)))\
         ^ (((b & 0xccccccccccccccccUL) >> 2) & (-((a >> 62) & 1UL)))\
         ^ (((b & 0x8888888888888888UL) >> 3) & (-((a >> 61) & 1UL)));\
      cp[i] = carry ^ lo;    carry = hi;\
   }\
   cp[sb] = carry;\





#define NTL_ALT1_BB_MUL_CODE2 \
   long i;\
   _ntl_ulong carry = 0, b;\
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      b = bp[i];\
      lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
      t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
      t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
      t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
      t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
      t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
      t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
      t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
      t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
      t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
      t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
      t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
      t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
      t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
      t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
      hi ^= (((b & 0xeeeeeeeeeeeeeeeeUL) >> 1) & (-(a >> 63)))\
         ^ (((b & 0xccccccccccccccccUL) >> 2) & (-((a >> 62) & 1UL)))\
         ^ (((b & 0x8888888888888888UL) >> 3) & (-((a >> 61) & 1UL)));\
      cp[i] ^= (carry ^ lo);    carry = hi;\
   }\
   cp[sb] ^= carry;\





#define NTL_ALT1_SHORT_BB_MUL_CODE1 \
   long i;\
   _ntl_ulong carry = 0, b;\
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[16];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   A[4] = A[2] << 1;\
   A[5] = A[4] ^ A[1];\
   A[6] = A[3] << 1;\
   A[7] = A[6] ^ A[1];\
   A[8] = A[4] << 1;\
   A[9] = A[8] ^ A[1];\
   A[10] = A[5] << 1;\
   A[11] = A[10] ^ A[1];\
   A[12] = A[6] << 1;\
   A[13] = A[12] ^ A[1];\
   A[14] = A[7] << 1;\
   A[15] = A[14] ^ A[1];\
   for (i = 0; i < sb; i++) {\
      b = bp[i];\
      lo = A[b & 15]; t = A[(b >> 4) & 15]; hi = t >> 60; lo ^= t << 4;\
      t = A[(b >> 8) & 15]; hi ^= t >> 56; lo ^= t << 8;\
      t = A[(b >> 12) & 15]; hi ^= t >> 52; lo ^= t << 12;\
      t = A[(b >> 16) & 15]; hi ^= t >> 48; lo ^= t << 16;\
      t = A[(b >> 20) & 15]; hi ^= t >> 44; lo ^= t << 20;\
      t = A[(b >> 24) & 15]; hi ^= t >> 40; lo ^= t << 24;\
      t = A[(b >> 28) & 15]; hi ^= t >> 36; lo ^= t << 28;\
      t = A[(b >> 32) & 15]; hi ^= t >> 32; lo ^= t << 32;\
      t = A[(b >> 36) & 15]; hi ^= t >> 28; lo ^= t << 36;\
      t = A[(b >> 40) & 15]; hi ^= t >> 24; lo ^= t << 40;\
      t = A[(b >> 44) & 15]; hi ^= t >> 20; lo ^= t << 44;\
      t = A[(b >> 48) & 15]; hi ^= t >> 16; lo ^= t << 48;\
      t = A[(b >> 52) & 15]; hi ^= t >> 12; lo ^= t << 52;\
      t = A[(b >> 56) & 15]; hi ^= t >> 8; lo ^= t << 56;\
      t = A[b >> 60]; hi ^= t >> 4; lo ^= t << 60;\
      cp[i] = carry ^ lo;    carry = hi;\
   }\
   cp[sb] = carry;\





#define NTL_ALT1_HALF_BB_MUL_CODE0 \
   _ntl_ulong hi, lo, t;\
   _ntl_ulong A[4];\
   A[0] = 0;\
   A[1] = a;\
   A[2] = A[1] << 1;\
   A[3] = A[2] ^ A[1];\
   lo = A[b & 3]; t = A[(b >> 2) & 3]; hi = t >> 62; lo ^= t << 2;\
   t = A[(b >> 4) & 3]; hi ^= t >> 60; lo ^= t << 4;\
   t = A[(b >> 6) & 3]; hi ^= t >> 58; lo ^= t << 6;\
   t = A[(b >> 8) & 3]; hi ^= t >> 56; lo ^= t << 8;\
   t = A[(b >> 10) & 3]; hi ^= t >> 54; lo ^= t << 10;\
   t = A[(b >> 12) & 3]; hi ^= t >> 52; lo ^= t << 12;\
   t = A[(b >> 14) & 3]; hi ^= t >> 50; lo ^= t << 14;\
   t = A[(b >> 16) & 3]; hi ^= t >> 48; lo ^= t << 16;\
   t = A[(b >> 18) & 3]; hi ^= t >> 46; lo ^= t << 18;\
   t = A[(b >> 20) & 3]; hi ^= t >> 44; lo ^= t << 20;\
   t = A[(b >> 22) & 3]; hi ^= t >> 42; lo ^= t << 22;\
   t = A[(b >> 24) & 3]; hi ^= t >> 40; lo ^= t << 24;\
   t = A[(b >> 26) & 3]; hi ^= t >> 38; lo ^= t << 26;\
   t = A[(b >> 28) & 3]; hi ^= t >> 36; lo ^= t << 28;\
   t = A[b >> 30]; hi ^= t >> 34; lo ^= t << 30;\
   hi ^= (((b & 0xaaaaaaaaUL) >> 1) & (-(a >> 63)));\
   c[0] = lo;    c[1] = hi;\



#define NTL_BB_MUL1_BITS (4)



#define NTL_BB_SQR_CODE \
lo=sqrtab[a&255];\
lo=lo|(sqrtab[(a>>8)&255]<<16);\
lo=lo|(sqrtab[(a>>16)&255]<<32);\
lo=lo|(sqrtab[(a>>24)&255]<<48);\
hi=sqrtab[(a>>32)&255];\
hi=hi|(sqrtab[(a>>40)&255]<<16);\
hi=hi|(sqrtab[(a>>48)&255]<<32);\
hi=hi|(sqrtab[(a>>56)&255]<<48);\




#define NTL_BB_REV_CODE (revtab[(a>>0)&255]<<56)\
|(revtab[(a>>8)&255]<<48)\
|(revtab[(a>>16)&255]<<40)\
|(revtab[(a>>24)&255]<<32)\
|(revtab[(a>>32)&255]<<24)\
|(revtab[(a>>40)&255]<<16)\
|(revtab[(a>>48)&255]<<8)\
|(revtab[(a>>56)&255]<<0)

#define NTL_MIN_LONG (-NTL_MAX_LONG - 1L)
#define NTL_MIN_INT  (-NTL_MAX_INT - 1)
#endif

