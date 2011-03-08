#ifndef COEFFS_H
#define COEFFS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL        (0)
#endif

#if (SIZEOF_LONG == 8)
typedef int BOOLEAN;
/* testet on x86_64, gcc 3.4.6: 2 % */
/* testet on IA64, gcc 3.4.6: 1 % */
#else
/* testet on athlon, gcc 2.95.4: 1 % */
typedef short BOOLEAN;
#endif
#define loop for(;;)

#if defined(SI_CPU_I386) || defined(SI_CPU_X86_64)
  // the following settings seems to be better on i386 and x86_64 processors
  // define if a*b is with mod instead of tables
  #define HAVE_MULT_MOD
  // #define HAVE_GENERIC_ADD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#elif defined(SI_CPU_IA64)
  // the following settings seems to be better on itanium processors
  // #define HAVE_MULT_MOD
  #define HAVE_GENERIC_ADD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#elif defined(SI_CPU_SPARC)
  // #define HAVE_GENERIC_ADD
  #define HAVE_MULT_MOD
  #ifdef HAVE_MULT_MOD
  #define HAVE_DIV_MOD
  #endif
#elif defined(SI_CPU_PPC)
  // the following settings seems to be better on ppc processors
  // testet on: ppc_Linux, 740/750 PowerMac G3, 512k L2 cache
  #define HAVE_MULT_MOD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#endif

enum n_coeffType
{
  n_unknown=0,
  n_Zp,
  n_Q,
  n_R,
  n_GF,
  n_long_R,
  n_Zp_a,
  n_Q_a,
  n_long_C
#ifdef HAVE_RINGS
  ,n_Z,
  n_Zm,
  n_Zpn,
  n_Z2n
#endif
};

struct n_Procs_s;
typedef struct  n_Procs_s  n_Procs_s;
typedef struct  n_Procs_s  *coeffs;

struct snumber;
typedef struct snumber *   number;
typedef number (*numberfunc)(number a,number b, const coeffs r);
typedef number (*nMapFunc)(number a, const coeffs r);

struct n_Procs_s
{
   coeffs next;
   // the union stuff

   // Zp:
   int npPrimeM;
   int npPminus1M;
   #ifdef HAVE_DIV_MOD
   unsigned short *npInvTable;
   #endif
   #if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
   unsigned short *npExpTable;
   unsigned short *npLogTable;
   #endif
   // Zp_a, Q_a

   // general stuff
   numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;
   number  (*cfInit)(int i,const coeffs r);
   number  (*nPar)(int i, const coeffs r);
   int     (*nParDeg)(number n, const coeffs r);
   int     (*nSize)(number n, const coeffs r);
   int     (*n_Int)(number &n, const coeffs r);
#ifdef HAVE_RINGS
   int     (*nDivComp)(number a,number b);
   BOOLEAN (*nIsUnit)(number a);
   number  (*nGetUnit)(number a);
   number  (*nExtGcd)(number a, number b, number *s, number *t);
#endif
   number  (*nNeg)(number a, const coeffs r);
   number  (*nInvers)(number a, const coeffs r);
   number  (*nCopy)(number a);
   number  (*cfCopy)(number a, const coeffs r);
   number  (*nRePart)(number a, const coeffs r);
   number  (*nImPart)(number a, const coeffs r);
   void    (*cfWrite)(number &a, const coeffs r);
   const char *  (*nRead)(const char * s, number * a, const coeffs r);
   void    (*nNormalize)(number &a, const coeffs r);
   BOOLEAN (*nGreater)(number a,number b, const coeffs r),
#ifdef HAVE_RINGS
           (*nDivBy)(number a, number b),
#endif
           (*nEqual)(number a,number b, const coeffs r),
           (*nIsZero)(number a, const coeffs r),
           (*nIsOne)(number a, const coeffs r),
           (*nIsMOne)(number a, const coeffs r),
           (*nGreaterZero)(number a, const coeffs r);
   void    (*nPower)(number a, int i, number * result, const coeffs r);
   number  (*cfGetDenom)(number &n, const coeffs r);
   number  (*cfGetNumerator)(number &n, const coeffs r);
   number  (*nGcd)(number a, number b, const coeffs r);
   number  (*nLcm)(number a, number b, const coeffs r);
   void    (*cfDelete)(number * a, const coeffs r);
   nMapFunc (*cfSetMap)(const coeffs src, const coeffs dst);
   char *  (*nName)(number n, const coeffs r);
   void    (*nInpMult)(number &a, number b, const coeffs r);
   number  (*nInit_bigint)(number i, const coeffs r);
#ifdef LDEBUG
   BOOLEAN (*nDBTest)(number a, const char *f,const int l);
#endif

   number nNULL; /* the 0 as constant */
   int     char_flag;
   int     ref;
   n_coeffType type;
   short   nChar;
////-----------------------------------------
  char**     parameter; /* names of parameters, rInit */
  number     minpoly;  /* for Q_a/Zp_a, rInit */

#ifdef HAVE_RINGS
  unsigned int  ringtype;  /* cring = 0 => coefficient field, cring = 1 => coeffs from Z/2^m */
  int_number    ringflaga; /* Z/(ringflag^ringflagb)=Z/nrnModul*/
  unsigned long ringflagb;
  unsigned long nr2mModul;  /* Z/nr2mModul */
  int_number    nrnModul;
#endif
  int        ch;  /* characteristic, rInit */

  short      float_len; /* additional char-flags, rInit */
  short      float_len2; /* additional char-flags, rInit */

};

#define  nInternalChar(r) ((r)->ch)

#endif

