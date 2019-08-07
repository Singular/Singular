#ifndef _SINGULAR_GLOBALDEFS_H
#define _SINGULAR_GLOBALDEFS_H

#ifndef PSINGULAR
#define THREAD_VAR
#define THREAD_INST_VAR
#define GLOBAL_VAR
#else
#define GLOBAL_VAR
#define THREAD_VAR __thread
#endif

#endif // _SINGULAR_GLOBALDEFS_H
