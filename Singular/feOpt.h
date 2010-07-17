#ifndef FEOPTS_H
#define FEOPTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: Declarations for working with Options
*/

#include <kernel/fegetopt.h>

extern const char SHORT_OPTS_STRING[];
#define LONG_OPTION_RETURN 13

/* specifies format of options */
extern struct fe_option feOptSpec[];

#ifndef GENERATE_OPTION_INDEX

/* provides feOptIndex enum type for fast accesses to feOptSpec */
#if ! defined(GENTABLE) && ! defined(GENERATE_DEPEND)
#ifdef ESINGULAR
#include <Singular/feOptES.inc>
#elif defined(TSINGULAR)
#include <Singular/feOptTS.inc>
#else
#include <Singular/feOpt.inc>
#endif
#else
typedef enum {FE_OPT_UNDEF}  feOptIndex;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* feGetOptValue(feOptIndex opt);


#ifdef __cplusplus
}

inline void* feOptValue(feOptIndex opt)
{
  return feOptSpec[(int)opt].value;
}
inline int feOptValue(feOptIndex opt, char** val)
{
  if (opt != FE_OPT_UNDEF && feOptSpec[(int)opt].type == feOptString)
  {
    *val = (char*) feOptSpec[(int)opt].value;
    return TRUE;
  }
  *val = NULL;
  return FALSE;
}
inline int feOptValue(feOptIndex opt, int* val)
{
  if (opt != FE_OPT_UNDEF && feOptSpec[(int)opt].type != feOptString)
  {
    *val = (int) ((long)(feOptSpec[(int)opt].value));
    return TRUE;
  }
  *val = 0;
  return FALSE;
}

// maps name to otions
feOptIndex feGetOptIndex(const char* name);
feOptIndex feGetOptIndex(int optc);

// Setting option values:
// Return: NULL -- everything ok
//         "error-string" on error

// opt->type must be feOptInt or feOptBool
const char* feSetOptValue(feOptIndex opt, int optarg);
// for opt->type != feOptString, optarg is converted
// to an int
const char* feSetOptValue(feOptIndex opt, char* optarg);

void fePrintOptValues();

#endif /* __cplusplus */

#endif /* ! GENERATE_OPTION_INDEX */

#endif /*  FEOPTS_H */
