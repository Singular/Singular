/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    rlimit.h
 * Purpose: set resource limits
 ***************************************************************/

#ifndef RLIMIT_H
#define RLIMIT_H

#ifdef __cplusplus
extern "C"
{
#endif

int raise_rlimit_nproc(void);

#ifdef __cplusplus
}
#endif

#endif
