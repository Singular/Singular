/*
 * $Id: proc_setup.cc,v 1.2 2000-01-21 09:23:22 krueger Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "modgen.h"
#include <mod2.h>
#include <tok.h>
#include "typmap.h"


#if 1
#  define logx printf
#else
#  define logx
#endif
/*#define DEBUG 1*/

static char default_do_typecheck = 1;
static char default_do_return = 1;

/*========================================================================*/
int init_proc(
  procdefv p,
  char *procname,
  paramdefv ret,
  int lineno
  )
{
  int i;
  
  if( p == NULL) return -1;
  if( p->procname != NULL ) free(p->procname);
  
  if( p->funcname != NULL ) free(p->funcname);
  if( p->c_code != NULL ) free(p->c_code);
  if( p->paramcnt>0 ) { 
    //free(p->param);
  }

  memset((void *)p, 0, sizeof(procdef));
  p->procname = strdup(procname);
  if( ret != NULL ) {
    char typname[32];

    decl2str(ret->typ, typname);
    ret->typname = strdup(typname);
    memcpy((void *)(&p->return_val), (void *)ret, sizeof(paramdef));
  }
  p->lineno = lineno;
  p->return_val.typ = NONE;
  p->flags.do_typecheck = default_do_typecheck;
  p->flags.do_return = default_do_return;
  p->flags.declaration_done = 0;
  p->flags.typecheck_done = 0;
  printf("Default: %d %d\n",  p->flags.do_typecheck,
         p->flags.do_return);
  
  return 0;
}

/*========================================================================*/
void AddParam(
  procdefv p,
  paramdefv vnew
  )
{
  paramdef pnew;
  int paramcnt = 0;
  char typname[32];
  
  memset(typname, 0, sizeof(typname));
  decl2str(vnew->typ, typname);
  
#if DEBUG
  logx("AddParam(%d, %s, %s, %d)\n", p->paramcnt, p->procname,
       typname, vnew->typ);
#endif

  memset((void *)&pnew, '\0', sizeof(paramdef));
  pnew.name = (char *)malloc(strlen(vnew->name)+1);
  memset(pnew.name, '\0', strlen(vnew->name)+1);
//   pnew.next = NULL;
  memcpy(pnew.name, vnew->name, strlen(vnew->name));
  pnew.typname = (char *)malloc(strlen(typname)+1);
  memset(pnew.typname, '\0', strlen(typname)+1);
  memcpy(pnew.typname, typname, strlen(typname));
  pnew.typ = vnew->typ;

  paramcnt = p->paramcnt;
 if(p->paramcnt==0) {
    p->param = (paramdefv)malloc(sizeof(paramdef));
 }
  else {
    p->param =
      (paramdefv)realloc(p->param,
                         (paramcnt+1)*sizeof(paramdef));
  }
  
  memcpy((void *)(&p->param[paramcnt]),
         (void *)&pnew, sizeof(paramdef));
  (p->paramcnt)++;
#if DEBUG
  logx("AddParam() done\n");
#endif
}

/*========================================================================*/
void proc_set_default_var(
  var_type type,
  var_token varid,
  char *varname,
  void *varvalue
  )
{
  int *i;
  char *q;

  switch(type) {
      case VAR_NUM:
      case VAR_BOOL:
        i = (int *)varvalue;
        switch(varid) {
            case VAR_TYPECHECK: default_do_typecheck = *i; break;
            case VAR_RETURN:    default_do_return = *i; break;
            default:            break;
        }
        break;
      case VAR_STRING:
        q = (char *)varvalue;
        switch(varid) {
        }
        break;
      case VAR_FILE:
        q = (char *)varvalue;
        printf("'%s'", q);
        break;

      case VAR_FILES:
        break;
  }
  printf("\n");
  
  return;
}

/*========================================================================*/
void proc_set_var(
  procdefv p,
  var_type type,
  var_token varid,
  char *varname,
  void *varvalue
  )
{
  int *i;
  char *q;
  
  
//  printf(">>>>>>>>Test 4 %s()[%d] '%s' = ", p->procname, type, varname);
  fflush(stdout);
  switch(type) {
      case VAR_NUM:
      case VAR_BOOL:
        i = (int *)varvalue;
        switch(varid) {
            case VAR_TYPECHECK: p->flags.do_typecheck = *i; break;
            case VAR_RETURN:    p->flags.do_return = *i; break;
            default:            break;
        }
        break;

      case VAR_STRING:
        q = (char *)varvalue;
        switch(varid) {
            case VAR_FUNCTION:
              if(p->funcname != NULL) free(p->funcname);
              p->funcname = strdup(q);
              break;
            case VAR_HELP:
              break;
        }
        break;

      case VAR_FILE:
        q = (char *)varvalue;
        //printf("'%s'", q);
        break;

      case VAR_FILES:
        break;
  }
//  printf("\n");
  
  return;
}

/*========================================================================*/
