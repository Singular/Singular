/*
 * $Id: proc.cc,v 1.1 1999-12-21 12:15:42 krueger Exp $
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

extern void PrintProc(procdefv pi);
void write_function(moddefv module, procdefv proc);

#define SELF_CMD MAX_TOK+1

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
  
  return 0;
}

/*========================================================================*/
void setup_proc(
  moddefv module,
  procdefv p,
  char *code
)
{

  if( p == NULL ) return;
  p->c_code = strdup(code);
  write_function(module, p);
  
//  PrintProc(p);
  return;
}

 /*========================================================================*/
static void write_function_declaration(procdefv pi, FILE *fp)
{
  int i;
  
  fprintf(fp, "BOOLEAN mod_%s(leftv res, leftv h);\n", pi->funcname);
  switch( pi->return_val.typ) {
      case SELF_CMD:
        fprintf(fp, "BOOLEAN %s(res, ", pi->funcname);
        break;

      default:
        fprintf(fp, "%s %s(", type_conv[pi->return_val.typ],
                pi->funcname);
  }
  for (i=0;i<pi->paramcnt; i++) {
    fprintf(fp, "%s res%d", type_conv[pi->param[i].typ], i);
    if(i<pi->paramcnt-1) fprintf(fp, ", ");
  }
  fprintf(fp, ");\n\n");
}

/*========================================================================*/
void write_function(
  moddefv module,
  procdefv proc
)
{
  if(proc->funcname == NULL) proc->funcname = strdup(proc->procname);
  if(proc->return_val.typ == NONE) {
    //if(proc->return_val.name == NULL) proc->return_val.name = strdup("none");
    if(proc->return_val.typname == NULL) {
    proc->return_val.typname = strdup("NONE");
    }
  }
  
  fprintf(module->modfp, "#line %d \"%s\"\n", proc->lineno, module->filename);
  fprintf(module->modfp, "  iiAddCproc(\"%s\",\"%s\",%s, mod_%s);\n",
          module->name, proc->procname, 
          proc->is_static ? "TRUE" : "FALSE",
          proc->funcname);
  generate_function(proc, module->fmtfp);
  write_function_declaration(proc, module->modfp_h);
  return;
}

/*========================================================================*/
void write_finish_functions(
  moddefv module,
  procdefv proc
)
{
  fprintf(module->modfp, "  return 0;\n}\n\n");
  fflush(module->modfp);
  
  mod_copy_tmp(module->modfp, module->fmtfp);
  printf("  done.\n");fflush(stdout);
  fclose(module->fmtfp);
  return;
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

