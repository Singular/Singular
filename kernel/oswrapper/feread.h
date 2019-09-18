#ifndef FEREAD_H
#define FEREAD_H
/****************************************
 * *  Computer Algebra System SINGULAR     *
 * ****************************************/
/*
 * ABSTRACT: terminal input
 */

#include "kernel/structs.h"

THREAD_VAR extern char    prompt_char; /*1 either '>' or '.'*/

#ifdef __cplusplus

/* the interface for reading: */
extern "C"  char * (*fe_fgets_stdin)(const char *pr,char *s, int size);

#ifdef HAVE_DYN_RL
char * fe_fgets_stdin_drl(const char *pr,char *s, int size);
#endif

extern "C" void fe_reset_input_mode();

extern "C" {
#ifndef HAVE_ATEXIT
void fe_reset_fe (int i, void *v);
#else
void fe_reset_fe (void);
#endif
}

/* possible implementations: */
extern "C"
{
  /* readline, linked in: */
  char * fe_fgets_stdin_rl(const char *pr,char *s, int size);

  /* emulated readline: */
  char * fe_fgets_stdin_emu(const char *pr,char *s, int size);

  /* fgets: */
  char * fe_fgets(const char *pr,char *s, int size);

  /* dummy (for batch mode): */
  char * fe_fgets_dummy(const char *pr,char *s, int size);

}
const char *  eati(const char *s, int *i);

#endif
#endif

