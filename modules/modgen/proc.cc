/*
 * $Id: proc.cc,v 1.6 2000-02-14 21:44:40 krueger Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "modgen.h"
#include <mod2.h>
#include <tok.h>
#include "typmap.h"

extern int trace;

#if 1
#  define logx printf
#else
#  define logx
#endif
/*#define DEBUG 1*/

extern void PrintProc(procdefv pi);
//void write_function(moddefv module, procdefv proc);
static void write_function_declaration(procdefv pi, FILE *fp);
static void  write_procedure_header(moddefv module, procdefv pi, FILE *fmtfp);
static void gen_func_param_check(FILE *fp, procdefv pi, int i);


#define SELF_CMD MAX_TOK+1

/*========================================================================*/
void setup_proc(
  moddefv module,
  procdefv proc
)
{
  /* if proc is NULL, just return */
  if( proc == NULL ) return;

  if(trace) printf("\tcreating '%s'...", proc->procname); fflush(stdout);
  fprintf(module->modfp, "#line %d \"%s\"\n", proc->lineno, module->filename);

  switch(proc->language) {
      case LANG_C:
        //  if(proc->funcname == NULL) proc->funcname = strdup(proc->procname);
        if(proc->return_val.typ == NONE) {
          if(proc->return_val.typname == NULL) {
            proc->return_val.typname = strdup("NONE");
          }
        }
  
        printf("iiAddCproc %s is C\n", proc->procname);
        /* write call to add procname to list */
        fprintf(module->modfp, "  iiAddCproc(\"%s\",\"%s\",%s, mod_%s);\n",
                module->name, proc->procname, 
                proc->is_static ? "TRUE" : "FALSE",
                proc->procname);
        if(proc->help_string!=NULL) {
          fprintf(module->modfp, "  namespaceroot->push( IDPACKAGE(helphdl) ,");
          fprintf(module->modfp, " IDID(helphdl));\n");
          fprintf(module->modfp, "  enter_id(\"%s\",", proc->procname);
          fprintf(module->modfp, " \"%s\", STRING_CMD);\n", proc->help_string);
          fprintf(module->modfp, "  namespaceroot->push();\n");
        }
        fprintf(module->modfp, "\n");

        write_procedure_header(module, proc, module->fmtfp);
        fprintf(module->modfp_h,
                "BOOLEAN mod_%s(leftv res, leftv h);\n", proc->procname);
        break;
        
      case LANG_SINGULAR:
        fprintf(module->modfp,
                "  h = add_singular_proc(%s, %d, %ld, %ld, %s);\n",
                proc->procname, proc->lineno,
                proc->sing_start, proc->sing_end,
                proc->is_static ? "TRUE" : "FALSE");
        break;
  }
  
  //  printf(" done\n");
}

/*========================================================================*/
void write_example(
  moddefv module,
  procdefv proc
  )
{
  /* if proc is NULL, just return */
  if( proc == NULL ) return;

  if(proc->example_string!=NULL) {
    fprintf(module->modfp, "  namespaceroot->push( IDPACKAGE(examplehdl) ,");
    fprintf(module->modfp, " IDID(examplehdl));\n");
    fprintf(module->modfp, "  enter_id(\"%s\",", proc->procname);
    fprintf(module->modfp, " \"%s\", STRING_CMD);\n", proc->example_string);
    fprintf(module->modfp, "  namespaceroot->push();\n");
  }
}
/*========================================================================*/
/*
 * write declaration of function to file pointed by 'fp', usualy the
 * header file.
 *
 * BOOLEAN mod_<procname>(leftv res, leftv h);
 */
static void write_function_declaration(procdefv pi, FILE *fp)
{
  int i;
  
#if 0
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
#endif
}

/*========================================================================*/
void write_function_declaration(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  int i;
  if(pi->paramcnt>0) {
    fprintf(module->fmtfp, "  leftv v = h, v_save;\n");
    fprintf(module->fmtfp, "  int tok = NONE, index = 0;\n");
    for (i=0;i<pi->paramcnt; i++)
      fprintf(module->fmtfp, 
              "  sleftv s%s; leftv %s = &s%s;\n", pi->param[i].varname,
              pi->param[i].varname, pi->param[i].varname);
    fprintf(module->fmtfp, "\n");
    pi->flags.declaration_done = 1;
  }
}


void write_function_typecheck(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  write_procedure_typecheck(module, pi, module->fmtfp);
  pi->flags.typecheck_done = 1;
}

void write_function_return(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  int i;

  if(arg!=NULL) {
    if( pi->funcname != NULL ) free(pi->funcname);
    pi->funcname = strdup((char *)arg);
    printf("CMD: return(%s)\n", arg);

    /* write function declaration to header file */
    switch( pi->return_val.typ) {
        case SELF_CMD:
          fprintf(module->modfp_h, "BOOLEAN %s(res, ", pi->funcname);
          break;

        default:
          fprintf(module->modfp_h, "%s %s(", type_conv[pi->return_val.typ],
                  pi->funcname);
    }
    for (i=0;i<pi->paramcnt; i++) {
      fprintf(module->modfp_h, "%s %s", type_conv[pi->param[i].typ], 
              pi->param[i].varname);
      if(i<pi->paramcnt-1) fprintf(module->modfp_h, ", ");
    }
    fprintf(module->modfp_h, ");\n\n");
  }
  else  printf("CMD: return()\n");

  write_procedure_return(module, pi, module->fmtfp);
}


/*========================================================================*/
/*
 * write declaration of function to file pointed by 'fp', usualy the
 * <module>.cc file
 *
 * BOOLEAN mod_<procname>(leftv res, leftv h)
 * {
 *    block of automatic type-checking and conversation
 *
 *    c++Code if any in the definition-file
 *    
 */
static void  write_procedure_header(
  moddefv module,
  procdefv pi,
  FILE *fmtfp
)
{
  int cnt = 0, i;
  
  printf("header..."); fflush(stdout);
  
  fprintf(fmtfp, "#line %d \"%s\"\n", pi->lineno, module->filename);
  fprintf(fmtfp, "BOOLEAN mod_%s(leftv res, leftv h)\n{\n", pi->procname);
}

/*========================================================================*/
/*
 * write declaration of function to file pointed by 'fp', usualy the
 * <module>.cc file
 *
 * BOOLEAN mod_<procname>(leftv res, leftv h)
 * {
 *    block of automatic type-checking and conversation
 *
 *    c++Code if any in the definition-file
 *    
 */
void  write_procedure_typecheck(
  moddefv module,
  procdefv pi,
  FILE *fmtfp
)
{
  int cnt = 0, i;
  
  if(!pi->flags.declaration_done) return;
  
  printf("type check..."); fflush(stdout);
  
  if(pi->paramcnt>0) {
    /* loop over all parameters, for type-checking and conversation */
    for (i=0;i<pi->paramcnt; i++) gen_func_param_check(fmtfp, pi, i);

    fprintf(fmtfp, "  if(v!=NULL) { tok = v->Typ(); goto mod_%s_error; }\n",
            pi->procname);

    fprintf(fmtfp, "\n");
  }
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
/* automatic type-checking and conversation for one parameter */
static void gen_func_param_check(
  FILE *fp,
  procdefv pi,
  int i
  )
{
  fprintf(fp, "  if(v==NULL) goto mod_%s_error;\n", pi->procname);
  fprintf(fp, "  tok = v->Typ();\n");
  fprintf(fp, "  if((index=iiTestConvert(tok, %s))==0)\n",
          pi->param[i].typname);
  fprintf(fp, "     goto mod_%s_error;\n", pi->procname);
  fprintf(fp, "  v_save = v->next;\n");
  fprintf(fp, "  v->next = NULL;\n");
  fprintf(fp, "  if(iiConvert(tok, %s, index, v, %s))\n",
          pi->param[i].typname, pi->param[i].varname);
  fprintf(fp, "     goto mod_%s_error;\n", pi->procname);
  fprintf(fp, "  v = v_save;\n");
}

/*========================================================================*/
/*
 * write end of function to file pointed by 'fp', usualy the
 * <module>.cc file
 *
 * ...
 * 
 *
 */
void write_procedure_return(
  moddefv module,
  procdefv pi,
  FILE *fmtfp
)
{
  int i;
  
  printf("\nWriting return-block\n");
    
  printf("### RETURN: '%s' '%s' '%d'\n", pi->return_val.name,
         pi->return_val.typname, pi->return_val.typ);
  if(pi->funcname == NULL) {
    fprintf(fmtfp, "  res->rtyp = NONE;\n");
    fprintf(fmtfp, "  res->data = NULL;\n");
    fprintf(fmtfp, "  return TRUE;\n");
  }
  else
    switch( pi->return_val.typ) {
        case SELF_CMD:
          fprintf(fmtfp, "    return(%s(res", pi->funcname);
          for (i=0;i<pi->paramcnt; i++)
            fprintf(fmtfp, ", (%s) %s->Data()",
                    type_conv[pi->param[i].typ], pi->param[i].varname);
          fprintf(fmtfp, "));\n\n");
          break;

        case NONE:
          fprintf(fmtfp, "  res->rtyp = %s;\n", pi->return_val.typname);
          fprintf(fmtfp, "  res->data = NULL;\n");
          fprintf(fmtfp, "  return(%s(", pi->funcname);
          for (i=0;i<pi->paramcnt; i++) {
            fprintf(fmtfp, "(%s) %s->Data()",
                    type_conv[pi->param[i].typ], pi->param[i].varname);
            if(i<pi->paramcnt-1) fprintf(fmtfp, ", ");
          }
          fprintf(fmtfp, "));\n\n");
          break;
          
        default:
          fprintf(fmtfp, "  res->rtyp = %s;\n", pi->return_val.typname);
          fprintf(fmtfp, "  res->data = (void *)%s(", pi->funcname);
          for (i=0;i<pi->paramcnt; i++) {
            fprintf(fmtfp, "(%s) %s->Data()",
                    type_conv[pi->param[i].typ], pi->param[i].varname);
            if(i<pi->paramcnt-1) fprintf(fmtfp, ", ");
          }
          fprintf(fmtfp, ");\n");
          fprintf(fmtfp, "  if(res->data != NULL) return FALSE;\n");
          fprintf(fmtfp, "  else return TRUE;\n\n");
    }
}

/*========================================================================*/
/*
    if(pi->param[0].typ==SELF_CMD) {
      if(pi->c_code != NULL) fprintf(fp, "%s\n", pi->c_code);
  
      fprintf(fp, "  return(%s(res,h));\n", pi->funcname);
      fprintf(fp, "}\n\n");
    }
    else {
*/

void write_function_errorhandling(
  moddefv module,
  procdefv pi
  )
{
  int cnt = 0, i;
  
  switch(pi->language) {
      case LANG_C:
        if(pi->paramcnt>0) {
          if(pi->flags.typecheck_done) {
            printf("error handling..."); fflush(stdout);
            fprintf(module->fmtfp, "  mod_%s_error:\n", pi->procname);
            fprintf(module->fmtfp, "    Werror(\"%s(`%%s`) is not supported\", Tok2Cmdname(tok));\n",
                    pi->procname);
            fprintf(module->fmtfp, "    Werror(\"expected %s(", pi->procname);
            for (i=0;i<pi->paramcnt; i++) {
              fprintf(module->fmtfp, "'%s'", pi->param[i].name);
              if(i!=pi->paramcnt-1) fprintf(module->fmtfp, ",");
            }
            fprintf(module->fmtfp, ")\");\n");
            fprintf(module->fmtfp, "    return TRUE;\n");
          } 
        }
        fprintf(module->fmtfp, "}\n\n");
        break;
      case LANG_SINGULAR:
        fprintf(module->binfp, "}\n// end of procedure %s\n\n", pi->procname);
        fprintf(module->modfp, "  if( h != NULL) {\n");
        fprintf(module->modfp, "    IDPROC(h)->data.s.body_end = %ld;\n",
                ftell(module->binfp));
        fprintf(module->modfp, "    IDPROC(h)->data.s.proc_end = %ld;\n",
                ftell(module->binfp));
        fprintf(module->modfp, "  }\n");
        break;
  }
  
}

/*========================================================================*/
int write_singular_procedures(
  moddefv module,
  procdefv proc
  )
{
  if(module->binfp==NULL) {
    char *filename;

    filename = (char *)malloc(strlen(module->name)+5+4);
    sprintf(filename, "tmp/%s.bin", module->name);

    if( (module->binfp = fopen(filename, "w")) == NULL) {
      free(filename);
      return -1;
    }
    printf("Creating %s, ", filename);fflush(stdout);

    free(filename);
  }
  
  /* */
  fprintf(module->binfp, "// line %d \"%s\"\n", proc->lineno,
          module->filename);
  fprintf(module->binfp, "// proc %s\n", proc->procname);
  proc->sing_start = ftell(module->binfp);
  
  return 0;
}

/*========================================================================*/
void write_singular_parameter(
  moddefv module,
  int lineno,
  char *typname,
  char *varname
  )
{
  fprintf(module->binfp, "  parameter %s %s;\n", typname, varname);
}

/*========================================================================*/
void write_codeline(
  moddefv module,
  procdefv proc,
  char *line,
  int lineno
  )
{
  switch(proc->language) {
      case LANG_SINGULAR:
        if(lineno>=0)
          fprintf(module->binfp, "// #line %d \"%s\"\n",
					  lineno, module->filename);
        fprintf(module->binfp, "%s", line); break;

      case LANG_C: 
        if(lineno>=0)
          fprintf(module->fmtfp, "#line %d \"%s\"\n",
					  lineno, module->filename);
        fprintf(module->fmtfp, "%s", line); break;
  }
}

/*========================================================================*/
