#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

/* for omBin */
#include <omalloc/omalloc.h>
#ifdef HAVE_RINGS
#include <coeffs/si_gmp.h>
#endif

/* standard types */
#define BITSET  unsigned int


/* C++-part */
#ifdef __cplusplus
#include <misc/auxiliary.h>
#include <kernel/polys.h>
#include <polys/coeffrings.h>

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


#ifdef __cplusplus
extern "C" {
#endif
void  m2_end(int i);
#ifdef __cplusplus
}
#endif

#define loop for(;;)

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

extern omBin char_ptr_bin;

#endif

