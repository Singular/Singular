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

// #ifdef _TRY
#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif
// #endif



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
  n_long_C,
  // only used if HAVE_RINGS is defined
  n_Z,
  n_Zm,
  n_Zpn,
  n_Z2n
};

struct n_Procs_s;
typedef struct  n_Procs_s  n_Procs_s;
typedef struct  n_Procs_s  *coeffs;



struct snumber;
typedef struct snumber *   number;
typedef number (*numberfunc)(number a, number b, const coeffs r);

/// maps "a" from aRing into r
typedef number (*nMapFunc)(number a, const coeffs r, const coeffs aRing);

// #define _TRY

#ifdef _TRY
typedef coeffs ring;

extern ring currRing;
#endif

struct n_Procs_s
{
#ifdef _TRY
  ring cf; // this
#endif

   coeffs next;
   unsigned int  ringtype;  /* 0 => coefficient field, 1 => coeffs from Z/2^m */

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
   // ?
   // initialisation:
   void (*cfInitChar)(coeffs r, int parameter); // do one-time initialisations
   void (*cfKillChar)(coeffs r); //  undo all initialisations
                                // or NULL
   void (*cfSetChar)(const coeffs r); // initialisations after each ring change
                                // or NULL
   // general stuff
   numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;
   /// init with an integer
   number  (*cfInit)(int i,const coeffs r);
   number  (*nPar)(int i, const coeffs r);
   int     (*nParDeg)(number n, const coeffs r);
   /// how complicated, (0) => 0, or positive
   int     (*nSize)(number n, const coeffs r);
   /// convertion, 0 if impossible
   int     (*n_Int)(number &n, const coeffs r);
#ifdef HAVE_RINGS
   int     (*nDivComp)(number a,number b,const coeffs r);
   BOOLEAN (*nIsUnit)(number a,const coeffs r);
   number  (*nGetUnit)(number a,const coeffs r);
   number  (*nExtGcd)(number a, number b, number *s, number *t,const coeffs r);
#endif
   /// changes argument  inline: a:= -a
   number  (*nNeg)(number a, const coeffs r);
   /// return 1/a
   number  (*nInvers)(number a, const coeffs r);
   /// return a copy of a
   number  (*cfCopy)(number a, const coeffs r);
   number  (*nRePart)(number a, const coeffs r);
   number  (*nImPart)(number a, const coeffs r);
   void    (*cfWrite)(number &a, const coeffs r);
   const char *  (*nRead)(const char * s, number * a, const coeffs r);
   void    (*nNormalize)(number &a, const coeffs r);
   BOOLEAN (*nGreater)(number a,number b, const coeffs r),
#ifdef HAVE_RINGS
           (*nDivBy)(number a, number b, const coeffs r),
#endif
            /// tests
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

   /// For extensions (writes into global string buffer)
   char *  (*nName)(number n, const coeffs r);

   /// Inline: a := b
   void    (*nInpMult)(number &a, number b, const coeffs r);
   number  (*nInit_bigint)(number i, const coeffs src, const coeffs dummy);

#ifdef LDEBUG
   /// Test: is "a" a correct number?
   BOOLEAN (*nDBTest)(number a, const char *f,const int l,const coeffs r);
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
  int_number    ringflaga; /* Z/(ringflag^ringflagb)=Z/nrnModul*/
  unsigned long ringflagb;
  unsigned long nr2mModul;  /* Z/nr2mModul */
  int_number    nrnModul;
#endif
  int        ch;  /* characteristic, rInit */

  short      float_len; /* additional char-flags, rInit */
  short      float_len2; /* additional char-flags, rInit */
};

/// Returns the type of coeffs domain
static inline n_coeffType getCoeffType(const coeffs r)
{
  return r->type;
}

#define  nInternalChar(r) ((r)->ch)

#endif

