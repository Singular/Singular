/*! \file coeffs/numstats.h Count number operarions over coefficient rings, fields and other domains suitable for Singular polynomials

  Optional addition for the main interface for Singular coefficients,
  i.e. to n_[A-Z].* (..., const coeffs) functions
*/
#ifndef NUMSTATS_H
#define NUMSTATS_H

#include "misc/auxiliary.h"
#include "reporter/reporter.h"

#ifndef HAVE_NUMSTATS

// Nothing will happen if HAVE_NUMSTATS was not defined
#define ALL_STATISTIC(FUN)
#define STATISTIC(f) do{}while(0)
#else

// Otherwise we will treat the following wrappers:

#define ALL_STATISTIC(FUN) \
    FUN(n_Test); \
    FUN(n_Size); \
    FUN(n_Normalize); \
    FUN(n_NormalizeHelper); \
    FUN(n_GetChar);  \
    FUN(n_SetMap); \
    FUN(n_Delete); \
    FUN(n_Copy);  \
    FUN(n_Init); \
    FUN(n_InitMPZ); \
    FUN(n_Int); \
    FUN(n_MPZ); \
    FUN(n_Invers); \
    FUN(n_Div); \
    FUN(n_DivBy); \
    FUN(n_ExactDiv); \
    FUN(n_IntMod); \
    FUN(n_Mult); \
    FUN(n_InpMult); \
    FUN(n_Power); \
    FUN(n_Add); \
    FUN(n_InpAdd); \
    FUN(n_Sub); \
    FUN(n_InpNeg); \
    FUN(n_Equal); \
    FUN(n_IsZero); \
    FUN(n_IsOne); \
    FUN(n_IsMOne); \
    FUN(n_GreaterZero); \
    FUN(n_Greater); \
    FUN(n_DivComp); \
    FUN(n_IsUnit); \
    FUN(n_GetUnit); \
    FUN(n_CoeffRingQuot1); \
    FUN(n_Lcm); \
    FUN(n_Gcd); \
    FUN(n_ExtGcd); \
    FUN(n_XExtGcd); \
    FUN(n_SubringGcd); \
    FUN(n_EucNorm); \
    FUN(n_Ann); \
    FUN(n_QuotRem); \
    FUN(n_Farey); \
    FUN(n_ChineseRemainderSym); \
    FUN(n_RePart); \
    FUN(n_ImPart); \
    FUN(n_ParDeg); \
    FUN(n_Param); \
    FUN(n_NumberOfParameters); \
    FUN(n_ParameterNames); \
    FUN(n_GetDenom); \
    FUN(n_GetNumerator); \
    FUN(n_ClearContent); \
    FUN(n_ClearDenominators); \
    FUN(n_Read); \
    FUN(n_Write); \
    FUN(n_ReadFd); \
    FUN(n_WriteFd); \
    FUN(n_WriteLong); \
    FUN(n_WriteShort); \
    FUN(nCoeffString); \
    FUN(nCoeffName); \
    FUN(nSetChar); \
    FUN(nKillChar); \
    FUN(n_convFactoryNSingN); \
    FUN(n_convSingNFactoryN); \
    FUN(n_Random); \
    FUN(n_CoeffWrite)



struct SNumberStatistic
{
  public:
    SNumberStatistic(){ Init(); }

    inline void Init(const unsigned long defaultvalue = 0)
    {
#define _Z(F) this->F = defaultvalue
      ALL_STATISTIC(_Z);
      _Z(n_CancelOut);
#undef _Z
    }

    inline void Print() const
    {
#define _P(F) if(this->F > 0) ::Print("%21s: %13lu\n", # F, this->F)
      ALL_STATISTIC(_P);
      _P(n_CancelOut);
#undef _P
    }

#define _UL(F) unsigned long F
    ALL_STATISTIC(_UL);
    _UL(n_CancelOut);
#undef _UL
};

#define STATISTIC(F) {extern struct SNumberStatistic number_stats; number_stats.F += 1;}
#endif

/// set all counters to zero
static inline void number_stats_Init(const unsigned long defaultvalue = 0)
{
#ifndef HAVE_NUMSTATS
  WarnS("Please enable NUMSTATS first!");
  (void)(defaultvalue);
#else
  EXTERN_VAR struct SNumberStatistic number_stats;
  number_stats.Init(defaultvalue);
#endif
}

/// print out all counters
static inline void number_stats_Print(const char * const msg = NULL)
{
   ::Print("%s:\n", (msg == NULL) ? "Statistic about number operations" : msg);
#ifndef HAVE_NUMSTATS
  WarnS("Please enable NUMSTATS first!");
#else
  EXTERN_VAR struct SNumberStatistic number_stats;
  number_stats.Print();
#endif
  ::PrintLn();
}
#endif /* NUMSTAT */

