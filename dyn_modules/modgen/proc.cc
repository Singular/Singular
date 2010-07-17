/*
 * $Id$
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "modgen.h"

#include "typmap.h"


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

  if(trace) printf("\n\tcreating '%s'...", proc->procname); fflush(stdout);
  fprintf(module->modfp, "#line %d \"%s\"\n", proc->lineno, module->filename);
  modlineno+=1;

  switch(proc->language)
  {
      case LANG_C:
        //  if(proc->funcname == NULL) proc->funcname = strdup(proc->procname);
        if(proc->return_val.typ == NONE) {
          if(proc->return_val.typname == NULL) {
            proc->return_val.typname = strdup("NONE");
          }
        }

        /* write call to add procname to list */
        fprintf(module->modfp,
                "  psModulFunctions->iiAddCproc(currPack->libname,\"%s\",%s, mod_%s);\n",
                proc->procname,
                proc->is_static ? "TRUE" : "FALSE",
                proc->procname);
        modlineno+=1;

        fprintf(module->modfp, "\n");
        modlineno+=1;

        write_procedure_header(module, proc, module->fmtfp);
        fprintf(module->modfp_h,
                "BOOLEAN mod_%s(leftv res, leftv h);\n", proc->procname);
        break;

      case LANG_SINGULAR:
        fprintf(module->modfp,"  if(ret!=-1) {\n");
        fprintf(module->modfp,
                "    h = add_singular_proc(binfp,\"%s\", %d, %ld, %ld, %s);\n",
                proc->procname, proc->lineno,
                proc->sing_start, proc->sing_end,
                proc->is_static ? "TRUE" : "FALSE");
        fprintf(module->modfp,"  }\n");
        modlineno+=1;
        break;
  }

}

/*========================================================================*/
void write_function_header(
  moddefv module,
  procdefv proc
  )
{
  if(!proc->flags.auto_header) return;
  if(!proc->flags.start_of_code) return;

  if(debug>4)
  {
    printf("write_function_header: auto=%d\n", proc->flags.auto_header);
    printf("write_function_header: declaration: do=%d, done=%d\n",
           proc->flags.do_declaration,
           proc->flags.declaration_done);
    printf("write_function_header: typecheck: do=%d, done=%d\n",
           proc->flags.do_typecheck,
           proc->flags.typecheck_done);
  }

  if(proc->flags.do_declaration && !proc->flags.declaration_done)
    write_function_declaration(module, proc, module->fmtfp);

  if(proc->flags.do_typecheck && !proc->flags.typecheck_done)
    write_function_typecheck(module, proc, module->fmtfp);
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
  switch( pi->return_val.typ)
  {
      case SELF_CMD:
        fprintf(fp, "BOOLEAN %s(res, ", pi->funcname);
        break;

      default:
        fprintf(fp, "%s %s(", type_conv[pi->return_val.typ],
                pi->funcname);
  }
  for (i=0;i<pi->paramcnt; i++)
  {
    fprintf(fp, "%s %s%d", type_conv[pi->param[i].typ], i);
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
  if(debug>4) printf("write_function_declaration: do=%d, done=%d\n",
                     pi->flags.do_declaration, pi->flags.declaration_done);
  if(pi->flags.declaration_done) return;
  if(!pi->flags.do_declaration) return;
  if(pi->paramcnt>0)
  {
    fprintf(module->fmtfp, "#line %d \"%s\"\n", yylineno, module->filename);
    fprintf(module->fmtfp, "#line @d \"%s.cc\"\n", module->name);
    fprintf(module->fmtfp, "  leftv __v = __h, __v_save;\n");
    fprintf(module->fmtfp, "  int __tok = NONE, __index = 0;\n");
    for (i=0;i<pi->paramcnt; i++)
    {
      fprintf(module->fmtfp,
              "  sleftv __s%s; leftv __z%s = &__s%s;\n", pi->param[i].varname,
              pi->param[i].varname, pi->param[i].varname);
      fprintf(module->fmtfp, "  %s %s;\n", type_conv[pi->param[i].typ],
              pi->param[i].varname);
    }

    fprintf(module->fmtfp, "#line %d \"%s\"\n", yylineno, module->filename);
    fprintf(module->fmtfp, "\n");
  }
  pi->flags.declaration_done = 1;
}

/*========================================================================*/
void write_function_nodecl(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  pi->flags.do_declaration = 0;
  pi->flags.do_typecheck = 0;
  pi->flags.auto_header = 0;
}

/*========================================================================*/
void write_function_error(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  int i;

  fprintf(module->fmtfp, "#line %d \"%s\"\n", yylineno, module->filename);
  fprintf(module->fmtfp, "#line @d \"%s.cc\"\n", module->name);
  fprintf(module->fmtfp, "#line %d \"%s\"\n", yylineno, module->filename);
  fprintf(module->fmtfp, "\n");
}

/*========================================================================*/
void write_function_typecheck(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  if(debug>4) printf("write_function_typecheck: do=%d, done=%d\n",
                     pi->flags.do_typecheck, pi->flags.typecheck_done);
  if(pi->flags.typecheck_done) return;
  if(!pi->flags.do_typecheck) return;

  fprintf(module->fmtfp, "#line %d \"%s\"\n", yylineno, module->filename);
  fprintf(module->fmtfp, "#line @d \"%s.cc\"\n", module->name);
  write_procedure_typecheck(module, pi, module->fmtfp);
  fprintf(module->fmtfp, "#line %d \"%s\"\n", yylineno, module->filename);
  pi->flags.typecheck_done = 1;
}

/*========================================================================*/
void write_function_result(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  fprintf(module->fmtfp, "  __res->data =%s\n", arg);
  pi->flags.result_done = 1;
  write_procedure_return(module, pi, module->fmtfp);
}

/*========================================================================*/
void write_function_return(
  moddefv module,
  procdefv pi,
  void *arg
  )
{
  int i;

  if(arg!=NULL)
  {
    if( pi->funcname != NULL ) free(pi->funcname);
    pi->funcname = strdup((char *)arg);
    if(debug>2)printf("CMD: return(%s)\n", arg);

    /* write function declaration to header file */
    switch( pi->return_val.typ)
    {
        case SELF_CMD:
          fprintf(module->modfp_h, "BOOLEAN %s(__res, ", pi->funcname);
          break;

        default:
          fprintf(module->modfp_h, "//%s %s(", type_conv[pi->return_val.typ],
                  pi->funcname);
    }
    for (i=0;i<pi->paramcnt; i++)
    {
      fprintf(module->modfp_h, "%s %s", type_conv[pi->param[i].typ],
              pi->param[i].varname);
      if(i<pi->paramcnt-1) fprintf(module->modfp_h, ", ");
    }
    fprintf(module->modfp_h, ");\n\n");
  }
  else
  {
    if(debug>2)printf("CMD: return()\n");
  }

  fprintf(module->fmtfp, "#line %d \"%s\"\n", yylineno, module->filename);
  write_procedure_return(module, pi, module->fmtfp);
}

/*========================================================================*/
void write_function_singularcmd(
  moddefv module,
  procdefv pi,
  void *arg
)
{
  int i;

  if(trace)printf("\n\t\tsingular command block...");
  fprintf(module->fmtfp, "#line @d \"%s.cc\"\n", module->name);
  fprintf(module->fmtfp, "/* code for running singular commands */\n");
  fprintf(module->fmtfp, "/*\n");
  fprintf(module->fmtfp, " * #line %d \"%s\"\n", yylineno, module->filename);
  fprintf(module->fmtfp, " *\n");
  fprintf(module->fmtfp, " * get idhdl for '%s'\n", arg);
  fprintf(module->fmtfp, " * building leftv of arguments\n");
  fprintf(module->fmtfp, " * sleftv cmdret = iiMake_proc(cmd, ???, sl)\n");
  fprintf(module->fmtfp, " *\n");
  fprintf(module->fmtfp, "### SINGULAR command: '%s'\n", arg);
  fprintf(module->fmtfp, "*/\n");
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

  if(trace)printf("\n\t\theader..."); fflush(stdout);

  fprintf(fmtfp, "#line %d \"%s\"\n", pi->lineno, module->filename);
#ifdef ix86_win
  fprintf(module->modfp,
          "  __declspec(dllexport) BOOLEAN mod_%s", pi->procname);
  fprintf("(leftv __res, leftv __h);\n");
#endif /* ix86_win */
  fprintf(fmtfp, "BOOLEAN mod_%s(leftv __res, leftv __h)\n{\n", pi->procname);
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

  if(trace)printf("type check..."); fflush(stdout);

  if(pi->paramcnt>0)
  {
    /* loop over all parameters, for type-checking and conversation */
    for (i=0;i<pi->paramcnt; i++)
      gen_func_param_check(fmtfp, pi, i);

    fprintf(fmtfp,
            "  if(__v!=NULL) { __tok = __v->Typ(); goto mod_%s_error; }\n",
            pi->procname);

    fprintf(fmtfp, "\n");

    if(trace)printf("\n\t\terror handling..."); fflush(stdout);

    fprintf(module->fmtfp, "  goto mod_%s_ok;\n", pi->procname);


    fprintf(module->fmtfp, "  mod_%s_error:\n", pi->procname);
    fprintf(module->fmtfp, "    Werror(\"%s(`%%s`) is not supported\", Tok2Cmdname(__tok));\n",
                    pi->procname);
    fprintf(module->fmtfp, "    Werror(\"expected %s(", pi->procname);
    for (i=0;i<pi->paramcnt; i++)
    {
      fprintf(module->fmtfp, "'%s'", pi->param[i].name);
      if(i!=pi->paramcnt-1) fprintf(module->fmtfp, ",");
    }
    fprintf(module->fmtfp, ")\");\n");
    fprintf(module->fmtfp, "    return TRUE;\n");

    fprintf(module->fmtfp, "  mod_%s_ok:\n", pi->procname);
  }
}

/*========================================================================*/
void write_finish_functions(
  moddefv module,
  procdefv proc
)
{
  fprintf(module->modfp, "  if(ret!=-1) fclose(binfp);\n");
  fprintf(module->modfp, "  return 0;\n}\n}\n\n");
  fflush(module->modfp);
  modlineno+=3;

  fprintf(module->modfp, "#line %d \"%s.cc\"\n", modlineno++, module->name);
  fprintf(module->modfp, "/* Help section */\n");
  fprintf(module->modfp, "void fill_help_package() {\n");
  modlineno+=3;
  mod_copy_tmp(module->modfp, module->fmtfp2);
  fprintf(module->modfp, "#line %d \"%s.cc\"\n", modlineno++, module->name);
  fprintf(module->modfp, "}  /* End of Help section */\n\n");
  modlineno+=3;

  fprintf(module->modfp, "/* Example section */\n");
  fprintf(module->modfp, "void fill_example_package() {\n");
  modlineno+=3;
  mod_copy_tmp(module->modfp, module->fmtfp3);
  fprintf(module->modfp, "#line %d \"%s.cc\"\n", modlineno++, module->name);
  fprintf(module->modfp, "} /* End of Example section */\n\n");
  modlineno+=2;

  mod_copy_tmp(module->modfp, module->fmtfp);
  fprintf(module->modfp, "#line %d \"%s.cc\"\n", modlineno++, module->name);
  if(trace)printf("  done.\n");fflush(stdout);
  fclose(module->fmtfp); module->fmtfp = NULL;
  fclose(module->fmtfp2); module->fmtfp2 = NULL;
  fclose(module->fmtfp3); module->fmtfp3 = NULL;
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
  fprintf(fp, "  if(__v==NULL) goto mod_%s_error;\n", pi->procname);
  fprintf(fp, "  __tok = __v->Typ();\n");
  fprintf(fp, "  if((__index=iiTestConvert(__tok, %s))==0)\n",
          pi->param[i].typname);
  fprintf(fp, "     goto mod_%s_error;\n", pi->procname);
  fprintf(fp, "  __v_save = __v->next;\n");
  fprintf(fp, "  __v->next = NULL;\n");
  fprintf(fp, "  if(iiConvert(__tok, %s, __index, __v, __z%s))\n",
          pi->param[i].typname, pi->param[i].varname);
  fprintf(fp, "     goto mod_%s_error;\n", pi->procname);
  fprintf(fp, "  __v = __v_save;\n");
  fprintf(fp, "  %s = (%s)(long)__z%s->Data();\n", pi->param[i].varname,
          type_conv[pi->param[i].typ], pi->param[i].varname);

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

  if(trace)printf("\n\t\treturn block...");

  if(debug>2)printf("### RETURN: '%s' '%s' '%d'\n", pi->return_val.name,
         pi->return_val.typname, pi->return_val.typ);
  fprintf(module->fmtfp, "#line @d \"%s.cc\"\n", module->name);
  if(pi->funcname == NULL)
  {
    if(!pi->flags.result_done) fprintf(fmtfp, "  __res->data = NULL;\n");
    fprintf(fmtfp, "  __res->rtyp = %s;\n", pi->return_val.typname);
    //fprintf(fmtfp, "  res->rtyp = NONE;\n");
    //fprintf(fmtfp, "  res->data = NULL;\n");
    fprintf(fmtfp, "  return FALSE;\n");
  }
  else
  {
    switch( pi->return_val.typ)
    {
        case SELF_CMD:
          fprintf(fmtfp, "    return(%s(__res", pi->funcname);
          for (i=0;i<pi->paramcnt; i++)
            fprintf(fmtfp, ", (%s) __%s->Data()",
                    type_conv[pi->param[i].typ], pi->param[i].varname);
          fprintf(fmtfp, "));\n\n");
          break;

        case NONE:
          if(!pi->flags.result_done) fprintf(fmtfp, "  __res->data = NULL;\n");
          fprintf(fmtfp, "  __res->rtyp = %s;\n", pi->return_val.typname);
          fprintf(fmtfp, "  return(%s(", pi->funcname);
          for (i=0;i<pi->paramcnt; i++) {
            fprintf(fmtfp, "(%s) __%s->Data()",
                    type_conv[pi->param[i].typ], pi->param[i].varname);
            if(i<pi->paramcnt-1) fprintf(fmtfp, ", ");
          }
          fprintf(fmtfp, "));\n\n");
          break;

        default:
          fprintf(fmtfp, "  __res->rtyp = %s;\n", pi->return_val.typname);
          fprintf(fmtfp, "  __res->data = (void *)%s;\n", pi->funcname);
          fprintf(fmtfp, "  if((__res->data != NULL)||(__res->rtyp==INT_CMD)) return FALSE;\n");
          fprintf(fmtfp, "  else return TRUE;\n\n");
    }
  }
}

/*========================================================================*/

/*========================================================================*/

void write_function_errorhandling(
  moddefv module,
  procdefv pi
  )
{
  int cnt = 0, i;

  switch(pi->language)
  {
      case LANG_C:
        fprintf(module->fmtfp, "}\n\n");
        break;
      case LANG_SINGULAR:
        fprintf(module->binfp, "}\n// end of procedure %s\n\n", pi->procname);
        break;
  }
}

/*========================================================================*/
void write_help(
  moddefv pModule,
  procdefv pi
  )
{
  unsigned int i;
  if(pi->help_string!=NULL)
  {
    fprintf(pModule->fmtfp2, "#line %d \"%s\"\n", pi->lineno_other,
            pModule->filename);
    fprintf(pModule->fmtfp2, "  enter_id(\"%s_help\",", pi->procname);
    fprintf(pModule->fmtfp2, " \"");
    for(i=0; i<strlen(pi->help_string); i++)
    {
        if(pi->help_string[i]=='\n') fprintf(pModule->fmtfp2,"\\n");
        else fprintf(pModule->fmtfp2,"%c", pi->help_string[i]);
    }
    fprintf(pModule->fmtfp2, "\", STRING_CMD);\n\n", pi->help_string);
  }
}

/*========================================================================*/
void write_example(
  moddefv module,
  procdefv pi
  )
{
  unsigned int i;
  /* if proc is NULL, just return */
  if( pi == NULL ) return;

  if(pi->example_string!=NULL)
  {
    fprintf(module->fmtfp3, "#line %d \"%s\"\n", pi->lineno_other,
            module->filename);
    fprintf(module->fmtfp3, "  enter_id(\"%s_example\",\n", pi->procname);
    fprintf(module->fmtfp3, " \"");
    for(i=0; i<strlen(pi->example_string); i++)
    {
        if(pi->example_string[i]=='\n') fprintf(module->fmtfp3,"\\n");
        else fprintf(module->fmtfp3,"%c", pi->example_string[i]);
    }
    fprintf(module->fmtfp3, "\", STRING_CMD);\n\n", pi->example_string);
  }
}

/*========================================================================*/
int write_singular_procedures(
  moddefv module,
  procdefv proc
  )
{
  if(module->binfp==NULL)
  {
    char filename[512];

    strcpy(filename, build_filename(module, module->targetname, 3));

    if( (module->binfp = fopen(filename, "w")) == NULL)
    {
      return -1;
    }
    if(trace)printf("Creating %s, ", filename);fflush(stdout);
  }

  /* */
  fprintf(module->binfp, "// line %d \"%s\"\n", proc->lineno,
          module->filename);
  fprintf(module->binfp, "// proc %s\n", proc->procname);
  fflush(module->binfp);
  proc->sing_start = ftell(module->binfp);

  return 0;
}

/*========================================================================*/
int write_helpfile_help(
  moddefv module,
  procdefv proc
  )
{
  if(module->docfp==NULL)
  {
    char filename[512];

    strcpy(filename, build_filename(module, module->targetname, 4));
    if( (module->docfp = fopen(filename, "w")) == NULL)
    {
      return -1;
    }
    if(trace)
    {
      printf("Creating %s, ", filename);fflush(stdout);
    }
    fprintf(module->docfp, "$library = \"%s.so\";\n",
           (module->targetname!=NULL) ? module->targetname : module->name);
    fprintf(module->docfp, "$version = \"%s\";\n",
           (module->revision!=NULL) ? module->revision : "none");
    fprintf(module->docfp, "$category = <<EOT;\n");
    fprintf(module->docfp, "%s\nEOT\n",
           (module->category!=NULL) ? module->category : "none");
    fprintf(module->docfp, "$info = <<EOT;\n");
    fprintf(module->docfp, "%s\nEOT\n",
           (module->info!=NULL) ? module->info : "none");
  }

  /* */
  fprintf(module->docfp, "push(@procs, \"%s\");\n",proc->procname);
  fprintf(module->docfp, "$help{\"%s\"} = <<EOT;\n",proc->procname);
  fprintf(module->docfp, "%s\nEOT\n",
            (proc->help_string!=NULL) ? proc->help_string :
            "No help available for this command");
  if(proc->example_string!=NULL)
  {
    fprintf(module->docfp, "$example{\"%s\"} = <<EOT;\n",proc->procname);
    fprintf(module->docfp, "%s\nEOT\n",proc->example_string);
  }
  fprintf(module->docfp, "$chksum{\"%s\"} = 0;\n", proc->procname);

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
void write_singular_end(
  moddefv module,
  procdefv proc,
  int lineno
  )
{
  fprintf(module->binfp, "%s.so\n",module->targetname);
  fflush(module->binfp);
  proc->sing_end = ftell(module->binfp);
}

/*========================================================================*/
void write_codeline(
  moddefv module,
  procdefv proc,
  char *line,
  int lineno
  )
{
  switch(proc->language)
  {
      case LANG_SINGULAR:
        if(lineno>=0)
          fprintf(module->binfp, "// #line %d \"%s\"\n",
                                          lineno, module->filename);
        fprintf(module->binfp,"%s",line);
        break;

      case LANG_C:
        write_function_header(module, proc);
        if(lineno>=0)
        {
          fprintf(module->fmtfp, "#line %d \"%s\"\n",
                  lineno, module->filename);
          fprintf(module->fmtfp, "#line @d \"%s.cc\"\n", module->name);
        }
        fprintf(module->fmtfp, "%s", line); break;
  }
}

/*========================================================================*/
