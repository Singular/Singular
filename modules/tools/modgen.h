/*
 *  $Id: modgen.h,v 1.2 1999-03-17 12:41:31 krueger Exp $
 *
 */

#define MOD_GEN_VERSION "0.2"

#define BUFLEN 128
#define TMPL_HEAD ""
#define TMPL_FOOT ""

class paramdef;
class procdef;
class moddef;
class cfiles;

typedef paramdef * paramdefv;
typedef procdef * procdefv;
typedef cfiles * cfilesv;
typedef moddef * moddefv;

class paramdef {
 public:
  paramdefv next;
  char *name;
  char *typname;
  int  typ;
};

class procdef {
 public:
  procdefv next;
  char *procname;
  char *funcname;
  int is_static;
  paramdefv param;
  int paramcnt;
};

class cfiles {
  public:
  cfilesv next;
  char *filename;
};

class moddef {
 public:
  char * name;
  unsigned int major, minor, level;
  char * version;
  char * revision;
  char * info;
  char * helpfile;
};

/*
 *
 */
extern int IsCmd(char *n, int & tok);
extern char * decl2str(int n, char *name);

extern procdefv Add2proclist(procdefv pi, char *name);
extern void AddParam(procdefv pi, char *name, char *typname, int typ);
extern void generate_mod(procdefv pi, moddefv module, cfilesv c_filelist);
extern void generate_function(procdefv pi, FILE *fp);
extern void mod_write_header(FILE *fp, char *module);
extern void generate_header(procdefv pi, FILE *fp);
extern void write_header(FILE *fp, char *module, char *comment="");
extern void mod_create_makefile(moddefv module, cfilesv c_filelist);
extern void make_version(char *p, moddefv module);
extern cfilesv Add2files(cfilesv cf, char *buff);
extern void PrintProclist(procdefv pi);

