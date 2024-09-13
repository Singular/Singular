#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

#include "kernel/mod2.h"

/* for omBin */
#include "omalloc/omalloc.h"

/* C++-part */
#ifdef __cplusplus
#include "kernel/polys.h"

class Voice;
class sleftv;
class procinfo;
class skStrategy;
class ssyStrategy;
class CPolynomialSummator;
class CGlobalMultiplier;
class CFormulaPowerMultiplier;
#endif


enum tHomog
{
  isNotHomog = FALSE,
  isHomog    = TRUE,
  testHomog
};

struct sip_package;
typedef struct sip_package ip_package;
typedef ip_package *       package;

typedef struct  n_Procs_s  n_Procs_s;

struct nc_struct;
typedef struct nc_struct   nc_struct;

typedef struct sip_link    ip_link;

/* the pointer types */
typedef char *             char_ptr;
typedef int  *             int_ptr;

#ifdef __cplusplus
typedef sleftv *           leftv;
typedef skStrategy *       kStrategy;
typedef ssyStrategy *      syStrategy;
typedef procinfo *         procinfov;

/* the function pointer types */

#endif /* __cplusplus */



/*
**  7. runtime procedures/global data
*/

/* 7.1 C-routines : */


#define loop for(;;)

EXTERN_VAR omBin char_ptr_bin;

#endif

