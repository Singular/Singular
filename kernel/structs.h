#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

/* for omBin */
#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#ifdef HAVE_RINGS
#include <kernel/si_gmp.h>
#endif

extern int dummy;

/* standard types */
#ifdef HAVE_RINGS
typedef unsigned long NATNUMBER;
typedef mpz_ptr int_number;
#endif

#define BITSET  unsigned int

#if SIZEOF_LONG == 4
typedef long long int64;
#elif SIZEOF_LONG == 8
typedef long int64;
#else
#error int64 undefined
#endif


/* C++-part */
#ifdef __cplusplus
class sleftv;
class procinfo;
class skStrategy;
class ssyStrategy;
class CPolynomialSummator;
class CGlobalMultiplier;
class CFormulaPowerMultiplier;
#endif

struct sip_package;
typedef struct sip_package ip_package;
typedef ip_package *       package;

typedef struct  n_Procs_s  n_Procs_s;

// #ifdef HAVE_PLURAL
struct nc_struct;
typedef struct nc_struct   nc_struct;
// #endif

typedef struct sip_link    ip_link;

/* the pointer types */
typedef char *             char_ptr;
typedef int  *             int_ptr;

#ifdef __cplusplus
//typedef ip_link *          si_link;
typedef sleftv *           leftv;
//typedef slists *           lists;
//typedef sattr *            attr;
typedef skStrategy *       kStrategy;
typedef ssyStrategy *      syStrategy;
typedef procinfo *         procinfov;

/* the function pointer types */

class idrec;
typedef idrec *   idhdl;

#endif /* __cplusplus */



/*
**  7. runtime procedures/global data
*/

/* 7.1 C-routines : */


#ifdef __cplusplus
extern "C" {
#endif
void  m2_end(int i) __attribute__((noreturn));
#ifdef __cplusplus
}
#endif

/* 7.2 C++-routines : */

#ifdef __cplusplus
int   inits(void);
int   IsPrime(int i);
extern int siSeed;
int siRand();
#endif

#define loop for(;;)

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

extern omBin char_ptr_bin;

#endif

