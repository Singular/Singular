#ifndef NTL_mach_desc__H
#define NTL_mach_desc__H


#define NTL_BITS_PER_LONG (32)
#define NTL_MAX_LONG (2147483647L)
#define NTL_MAX_INT (2147483647)
#define NTL_BITS_PER_INT (32)
#define NTL_ARITH_RIGHT_SHIFT (1)
#define NTL_NBITS_MAX (30)
#define NTL_DOUBLE_PRECISION (53)
#define NTL_FDOUBLE_PRECISION (((double)(1L<<30))*((double)(1L<<22)))
#define NTL_QUAD_FLOAT_SPLIT ((((double)(1L<<27)))+1.0)
#define NTL_EXT_DOUBLE (1)
#define NTL_SINGLE_MUL_OK (1)
#define NTL_DOUBLES_LOW_HIGH (1)




#define NTL_BB_MUL_CODE \
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>8)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>6)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>4)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>2)&3];\




#define NTL_BB_HALF_MUL_CODE \
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>8)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>6)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>4)&3];\
hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>2)&3];\




#define NTL_BB_SQR_CODE \
lo=sqrtab[a&255];\
lo=lo|(sqrtab[(a>>8)&255]<<16);\
hi=sqrtab[(a>>16)&255];\
hi=hi|(sqrtab[(a>>24)&255]<<16);\




#define NTL_BB_REV_CODE (revtab[(a>>0)&255]<<24)\
|(revtab[(a>>8)&255]<<16)\
|(revtab[(a>>16)&255]<<8)\
|(revtab[(a>>24)&255]<<0)

#define NTL_MIN_LONG (-NTL_MAX_LONG - 1L)
#define NTL_MIN_INT  (-NTL_MAX_INT - 1)
#endif

