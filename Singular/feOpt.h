#ifndef FEOPTS_H
#define FEOPTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: Declarations for working with Options
*/

#include "resources/fegetopt.h"

#include "Singular/feOptTab.h"

extern const char SHORT_OPTS_STRING[];

/* specifies format of options */
EXTERN_VAR struct fe_option feOptSpec[];

/* provides feOptIndex enum type for fast accesses to feOptSpec */
#if ! defined(GENERATE_DEPEND)

# ifdef ESINGULAR
#  include "Singular/feOptES.inc"
# elif defined(TSINGULAR)
#  include "Singular/feOptTS.inc"
# else
#  include "Singular/feOpt.inc"
# endif

#else
typedef enum {FE_OPT_UNDEF}  feOptIndex;
#endif


void feOptHelp(const char* name);
void feOptDumpVersionTuple(void);

#ifdef __cplusplus

static inline void* feOptValue(feOptIndex opt)
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

// maps name to options
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

#endif /*  FEOPTS_H */
