/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: help system
*/

#include "kernel/mod2.h"
#include "omalloc/omalloc.h"
#include "misc/mylimits.h"
#include "resources/feResource.h"
#include "reporter/reporter.h"
#include "resources/omFindExec.h"
#include "reporter/si_signals.h"

#include "ipid.h"
#include "ipshell.h"
#include "libparse.h"
#include "feOpt.h"

#include "tok.h"
#include "fehelp.h"

/*****************************************************************
 *
 * Declarations: Data  structures
 *
 *****************************************************************/
#define MAX_HE_ENTRY_LENGTH 160
typedef struct
{
  char key[MAX_HE_ENTRY_LENGTH];
  char node[MAX_HE_ENTRY_LENGTH];
  char url[MAX_HE_ENTRY_LENGTH];
  long  chksum;
} heEntry_s;
typedef  heEntry_s * heEntry;

typedef void (*heBrowserHelpProc)(heEntry hentry, int br);
typedef BOOLEAN (*heBrowserInitProc)(int warn, int br);

typedef struct
{
  const char* browser;
  heBrowserInitProc init_proc;
  heBrowserHelpProc help_proc;
  const char* required;
  const char* action;
} heBrowser_s;
typedef heBrowser_s * heBrowser;

/*****************************************************************
 *
 * Declarations: Local functions
 *
 *****************************************************************/
static char* strclean(char* str);
static BOOLEAN heKey2Entry(char* filename, char* key, heEntry hentry);
static int heReKey2Entry (char* filename, char* key, heEntry hentry);
static BOOLEAN strmatch(char* s, char* re);
static BOOLEAN heOnlineHelp(char* s);
static void heBrowserHelp(heEntry hentry);
static long heKeyChksum(char* key);

// browser functions
static BOOLEAN heGenInit(int,int);    static void heGenHelp(heEntry hentry,int);
                                      static void heBuiltinHelp(heEntry hentry,int);
static BOOLEAN heDummyInit(int,int);   static void heDummyHelp(heEntry hentry,int);
static BOOLEAN heEmacsInit(int,int);   static void heEmacsHelp(heEntry hentry,int);

STATIC_VAR heBrowser heCurrentHelpBrowser = NULL;
STATIC_VAR int heCurrentHelpBrowserIndex= -1;


/*****************************************************************
 *
 * Definition: available help browsers
 *
 *****************************************************************/
// order is important -- first possible help is chosen
// moved to LIB/help.cnf
STATIC_VAR heBrowser_s *heHelpBrowsers=NULL;

/*****************************************************************
 *
 * Implementation: public function
 *
 *****************************************************************/
void feHelp(char *str)
{
  str = strclean(str);
  if (str == NULL) {heBrowserHelp(NULL); return;}

  if (strlen(str) > MAX_HE_ENTRY_LENGTH - 2)  // need room for extra **
    str[MAX_HE_ENTRY_LENGTH - 3] = '\0';

  BOOLEAN key_is_regexp = (strchr(str, '*') != NULL);


  heEntry_s hentry;
  memset(&hentry,0,sizeof(hentry));
  char* idxfile = feResource('x' /*"IdxFile"*/);

  // Try exact match of help string with key in index
  if (!key_is_regexp && (idxfile != NULL) && heKey2Entry(idxfile, str, &hentry))
  {
    heBrowserHelp(&hentry);
    return;
  }

  // Try to match approximately with key in index file
  if (idxfile != NULL)
  {
    if (heCurrentHelpBrowser == NULL) feHelpBrowser(NULL, 0);
    assume(heCurrentHelpBrowser != NULL);

    StringSetS("");
    int found = heReKey2Entry(idxfile, str, &hentry);


    if (found == 0)
    {
      // try proc help and library help
      if (! key_is_regexp && heOnlineHelp(str)) return;

      // Try to match with str*
      char mkey[MAX_HE_ENTRY_LENGTH];
      strcpy(mkey, str);
      strcat(mkey, "*");
      found = heReKey2Entry(idxfile, mkey, &hentry);
      // Try to match with *str*
      if (found == 0)
      {
        mkey[0] = '*';
        strcpy(mkey + 1, str);
        strcat(mkey, "*");
        found = heReKey2Entry(idxfile, mkey, &hentry);
      }

      // Print warning and return if nothing found
      if (found == 0)
      {
        Warn("No help for topic '%s' (not even for '*%s*')", str, str);
        WarnS("Try '?;'       for general help");
        WarnS("or  '?Index;'  for all available help topics.");
        return;
      }
    }

    // do help if unique match was found
    if (found == 1)
    {
      heBrowserHelp(&hentry);
      return;
    }
    // Print warning about multiple matches and return
    if (key_is_regexp)
      Warn("No unique help for '%s'", str);
    else
      Warn("No help for topic '%s'", str);
    WarnS("Try one of");
    char *matches=StringEndS();
    PrintS(matches);
    omFree(matches);
    PrintLn();
    return;
  }

  // no idx file, let Browsers deal with it, if they can
  strcpy(hentry.key, str);
  *hentry.node = '\0';
  *hentry.url = '\0';
  hentry.chksum = 0;
  heBrowserHelp(&hentry);
}
static void feBrowserFile()
{
  FILE *f=feFopen("help.cnf","r",NULL,TRUE);
  int br=0;
  if (f!=NULL)
  {
    char buf[512];
    while (fgets( buf, sizeof(buf), f))
    {
      if ((buf[0]!='#') && (buf[0]>' ')) br++;
    }
    fseek(f,0,SEEK_SET);
    // for the 4(!) default browsers
    heHelpBrowsers=(heBrowser_s*)omAlloc0((br+4)*sizeof(heBrowser_s));
    br = 0;
    while (fgets( buf, sizeof(buf), f))
    {
      if ((buf[0]!='#') && (buf[0]>' '))
      {
        char *name=strtok(buf,"!");
        char *req=strtok(NULL,"!");
        char *cmd=strtok(NULL,"!");
        if ((name!=NULL) && (req!=NULL) && (cmd!=NULL))
        {
          while ((cmd[0]!='\0') && (cmd[strlen(cmd)-1]<=' '))
            cmd[strlen(cmd)-1]='\0';
          //Print("name %d >>%s<<\n\treq:>>%s<<\n\tcmd:>>%s<<\n",br,name,req,cmd);
          heHelpBrowsers[br].browser=(char *)omStrDup(name);
          heHelpBrowsers[br].init_proc=heGenInit;
          heHelpBrowsers[br].help_proc=heGenHelp;
          heHelpBrowsers[br].required=omStrDup(req);
          heHelpBrowsers[br].action=omStrDup(cmd);
          br++;
        }
        else
        {
          Print("syntax error in help.cnf, at line starting with %s\n",buf);
        }
      }
    }
    fclose(f);
  }
  else
  {
    // for the 4(!) default browsers
    heHelpBrowsers=(heBrowser_s*)omAlloc0(4*sizeof(heBrowser_s));
  }
  heHelpBrowsers[br].browser="builtin";
  heHelpBrowsers[br].init_proc=heGenInit;
  heHelpBrowsers[br].help_proc=heBuiltinHelp;
  heHelpBrowsers[br].required="i";
  //heHelpBrowsers[br].action=NULL;
  br++;
  heHelpBrowsers[br].browser="dummy";
  heHelpBrowsers[br].init_proc=heDummyInit;
  heHelpBrowsers[br].help_proc=heDummyHelp;
  //heHelpBrowsers[br].required=NULL;
  //heHelpBrowsers[br].action=NULL;
  br++;
  heHelpBrowsers[br].browser="emacs";
  heHelpBrowsers[br].init_proc=heEmacsInit;
  heHelpBrowsers[br].help_proc=heEmacsHelp;
  //heHelpBrowsers[br].required=NULL;
  //heHelpBrowsers[br].action=NULL;
  //br++;
  //heHelpBrowsers[br].browser=NULL;
  //heHelpBrowsers[br].init_proc=NULL;
  //heHelpBrowsers[br].help_proc=NULL;
  //heHelpBrowsers[br].required=NULL;
  //heHelpBrowsers[br].action=NULL;
}

const char* feHelpBrowser(char* which, int warn)
{
  int i = 0;

  // if no argument, choose first available help browser
  if (heHelpBrowsers==NULL) feBrowserFile();
  if (which == NULL || *which == '\0')
  {
    // return, if already set
    if (heCurrentHelpBrowser != NULL)
      return heCurrentHelpBrowser->browser;

    // First, try emacs, if emacs-option is set
    if (feOptValue(FE_OPT_EMACS) != NULL)
    {
      while (heHelpBrowsers[i].browser != NULL)
      {
        if (strcmp(heHelpBrowsers[i].browser, "emacs") == 0 &&
            (heHelpBrowsers[i].init_proc(0,i)))
        {
          heCurrentHelpBrowser = &(heHelpBrowsers[i]);
          heCurrentHelpBrowserIndex=i;
          goto Finish;
        }
        i++;
      }
      i=0;
    }
    while (heHelpBrowsers[i].browser != NULL)
    {
      if (heHelpBrowsers[i].init_proc(0,i))
      {
        heCurrentHelpBrowser = &(heHelpBrowsers[i]);
        heCurrentHelpBrowserIndex=i;
        goto Finish;
      }
      i++;
    }
    // should never get here
    dReportBug("should never get here");
  }

  // with argument, find matching help browser
  while (heHelpBrowsers[i].browser != NULL &&
         strcmp(heHelpBrowsers[i].browser, which) != 0)
  {i++;}

  if (heHelpBrowsers[i].browser == NULL)
  {
    if (warn) Warn("No help browser '%s' available.", which);
  }
  else
  {
    // see whether we can init it
    if (heHelpBrowsers[i].init_proc(warn,i))
    {
      heCurrentHelpBrowser = &(heHelpBrowsers[i]);
      heCurrentHelpBrowserIndex=i;
      goto Finish;
    }
  }

  // something went wrong
  if (heCurrentHelpBrowser == NULL)
  {
    feHelpBrowser();
    assume(heCurrentHelpBrowser != NULL);
    if (warn)
      Warn("Setting help browser to '%s'.", heCurrentHelpBrowser->browser);
    return heCurrentHelpBrowser->browser;
  }
  else
  {
    // or, leave as is
    if (warn)
      Warn("Help browser stays at '%s'.",  heCurrentHelpBrowser->browser);
    return heCurrentHelpBrowser->browser;
  }

  Finish:
  // update value of Browser Option
  if (feOptSpec[FE_OPT_BROWSER].value == NULL ||
      strcmp((char*) feOptSpec[FE_OPT_BROWSER].value,
             heCurrentHelpBrowser->browser) != 0)
  {
    omfree(feOptSpec[FE_OPT_BROWSER].value);
    feOptSpec[FE_OPT_BROWSER].value
     = (void*) omStrDup(heCurrentHelpBrowser->browser);
  }
  return heCurrentHelpBrowser->browser;
}

void  feStringAppendBrowsers(int warn)
{
  int i;
  StringAppendS("Available HelpBrowsers: ");

  i = 0;
  if (heHelpBrowsers==NULL) feBrowserFile();
  while (heHelpBrowsers[i].browser != NULL)
  {
    if (heHelpBrowsers[i].init_proc(warn,i))
      StringAppend("%s, ", heHelpBrowsers[i].browser);
    i++;
  }
  StringAppend("\nCurrent HelpBrowser: %s ", feHelpBrowser());
}


/*****************************************************************
 *
 * Implementation: local function
 *
 *****************************************************************/
// Remove whitspaces from beginning and end, return NULL if only whitespaces
static char* strclean(char* str)
{
  if (str == NULL) return NULL;
  char *s=str;
  while ((*s <= ' ') && (*s != '\0')) s++;
  if (*s == '\0') return NULL;
  char *ss=s;
  while (*ss!='\0') ss++;
  ss--;
  while ((*ss <= ' ') && (*ss != '\0'))
  {
    *ss='\0';
    ss--;
  }
  if (*ss == '\0') return NULL;
  return s;
}

// Finds help entry for key:
// returns filled-in hentry and TRUE, on success
// FALSE, on failure
// Assumes that lines of idx file have the following form
// key\tnode\turl\tchksum\n (chksum ma be empty, then it is set to -1)
// and that lines are sorted alpahbetically w.r.t. index entries
static BOOLEAN heKey2Entry(char* filename, char* key, heEntry hentry)
{
  FILE* fd;
  int c, k;
  int kl, i;
  *(hentry->key) = '\0';
  *(hentry->url) = '\0';
  *(hentry->node) = '\0';
  hentry->chksum = 0;
  if (filename == NULL || key == NULL)  return FALSE;
  fd = fopen(filename, "r");
  if (fd == NULL) return FALSE;
  kl = strlen(key);

  k = key[0];
  i = 0;
  while ((c = getc(fd)) != EOF)
  {
    if (c < k)
    {
      /* Skip line */
      while (getc(fd) != '\n') {};
      if (i)
      {
        i=0;
        k=key[0];
      }
    }
    else if (c == k)
    {
      i++;
      if (i == kl)
      {
        // \t must follow, otherwise only substring match
        if (getc(fd) != '\t') goto Failure;

        // Now we found an exact match
        if (hentry->key != key) strcpy(hentry->key, key);
        // get node
        i = 0;
        while ((c = getc(fd)) != '\t' && c != EOF)
        {
          hentry->node[i] = c;
          i++;
        }
        if (c == EOF) goto Failure;
        if (hentry->node[0]=='\0')
          strcpy(hentry->node,hentry->key);

        // get url
        //hentry->node[i] = '\0';
        i = 0;
        while ((c = getc(fd)) != '\t' && c != EOF)
        {
          hentry->url[i] = c;
          i++;
        }
        if (c == EOF) goto Failure;

        // get chksum
        hentry->url[i] = '\0';

        if (si_fscanf(fd, "%ld\n", &(hentry->chksum)) != 1)
        {
          hentry->chksum = -1;
        }
        fclose(fd);
        return TRUE;
      }
      else if (i > kl)
      {
        goto Failure;
      }
      else
      {
        k = key[i];
      }
    }
    else
    {
      goto Failure;
    }
  }
  Failure:
  fclose(fd);
  return FALSE;
}

// return TRUE if s matches re
// FALSE, otherwise
// does not distinguish lower and upper cases
// inteprets * as wildcard
static BOOLEAN strmatch(char* s, char* re)
{
  if (s == NULL || *s == '\0')
    return (re == NULL || *re == '\0' || strcmp(re, "*") == 0);
  if (re == NULL || *re == '\0') return FALSE;

  int i;
  char ls[MAX_HE_ENTRY_LENGTH + 1];
  char rs[MAX_HE_ENTRY_LENGTH + 1];
  char *l, *r, *ll, *rr;

  // make everything to lower case
  i=1;
  ls[0] = '\0';
  do
  {
    if (*s >= 'A' && *s <= 'Z') ls[i] = *s + ('a' - 'A');
    else ls[i] = *s;
    i++;
    s++;
  } while (*s != '\0');
  ls[i] = '\0';
  l = &(ls[1]);

  i=1;
  rs[0] = '\0';
  do
  {
    if (*re >= 'A' && *re <= 'Z') rs[i]= *re + ('a' - 'A');
    else rs[i] = *re;
    i++;
    re++;
  } while (*re != '\0');
  rs[i] = '\0';
  r = &(rs[1]);

  // chopp of exact matches from beginning and end
  while (*r != '*' && *r != '\0' && *l != '\0')
  {
    if (*r != *l) return FALSE;
    *r = '\0';
    *s = '\0';
    r++;
    l++;
  }
  if (*r == '\0') return (*l == '\0');
  if (*r == '*' && r[1] == '\0') return TRUE;
  if (*l == '\0') return FALSE;

  rr = &r[strlen(r) - 1];
  ll = &l[strlen(l) - 1];
  while (*rr != '*' && *rr != '\0' && *ll != '\0')
  {
    if (*rr != *ll) return FALSE;
    *rr = '\0';
    *ll = '\0';
    rr--;
    ll--;
  }
  if (*rr == '\0') return (*ll == '\0');
  if (*rr == '*' && rr[-1] == '\0') return TRUE;
  if (*ll == '\0') return FALSE;

  // now *r starts with a * and ends with a *
  r++;
  *rr = '\0'; rr--;
  while (*r != '\0')
  {
    rr = r + 1;
    while (*rr != '*' && *rr != '\0') rr++;
    if (*rr == '*')
    {
      *rr = '\0';
      rr++;
    }
    l = strstr(l, r);
    if (l == NULL) return FALSE;
    r = rr;
  }
  return TRUE;
}

// similar to heKey2Entry, except that
// key is taken as regexp (see above)
// and number of matches is returned
// if number of matches > 0, then hentry contains entry for first match
// if number of matches > 1, matches are printed as komma-separated
// into global string
static int heReKey2Entry (char* filename, char* key, heEntry hentry)
{
  int i = 0;
  FILE* fd;
  char index_key[MAX_HE_ENTRY_LENGTH];

  if (filename == NULL || key == NULL)  return 0;
  fd = fopen(filename, "r");
  if (fd == NULL) return 0;
  memset(index_key,0,MAX_HE_ENTRY_LENGTH);
  while (si_fscanf(fd, "%[^\t]\t%*[^\n]\n", index_key) == 1)
  {
    if ((index_key[MAX_HE_ENTRY_LENGTH-1]!='\0'))
    {
      index_key[MAX_HE_ENTRY_LENGTH-1]='\0';
      Werror("index file corrupt at line >>%s<<",index_key);
      break;
    }
    else if (strmatch(index_key, key))
    {
      i++;
      if (i == 1)
      {
        heKey2Entry(filename, index_key, hentry);
      }
      else if (i == 2)
      {
        StringAppend("?%s; ?%s;", hentry->key, index_key);
      }
      else
      {
        StringAppend(" ?%s;", index_key);
      }
    }
  }
  fclose(fd);
  return i;
}

// test for h being a string and print it
static void hePrintHelpStr(const idhdl hh,const char *id,const char *pa)
{
  if ((hh!=NULL) && (IDTYP(hh)==STRING_CMD))
  {
    PrintS(IDSTRING(hh));
    PrintLn();
  }
  else
    Print("`%s` not found in package %s\n",id,pa);
}
// try to find the help string as a loaded procedure or library
// if found, display the help and return TRUE
// otherwise, return FALSE
static BOOLEAN heOnlineHelp(char* s)
{
  char *ss;
  idhdl h;

  if ((ss=strstr(s,"::"))!=NULL)
  {
    *ss='\0';
    ss+=2;
    h=ggetid(s);
    if (h!=NULL)
    {
      Print("help for %s from package %s\n",ss,s);
      char s_help[200];
      strcpy(s_help,ss);
      strcat(s_help,"_help");
      idhdl hh=IDPACKAGE(h)->idroot->get(s_help,0);
      hePrintHelpStr(hh,s_help,s);
      return TRUE;
    }
    else Print("package %s not found\n",s);
    return TRUE; /* do not search the manual */
  }
  h=IDROOT->get(s,myynest);
  // try help for a procedure
  if (h!=NULL)
  {
    if  (IDTYP(h)==PROC_CMD)
    {
      char *lib=iiGetLibName(IDPROC(h));
      if((lib!=NULL)&&(*lib!='\0'))
      {
        Print("// proc %s from lib %s\n",s,lib);
        procinfov pi=IDPROC(h);
        if (pi->language==LANG_SINGULAR)
        {
          s=iiGetLibProcBuffer(pi, 0);
          if (s!=NULL)
          {
            PrintS(s);
            omFree((ADDRESS)s);
          }
          return TRUE;
        }
      }
    }
    else if (IDTYP(h)==PACKAGE_CMD)
    {
      idhdl hh=IDPACKAGE(h)->idroot->get("info",0);
      hePrintHelpStr(hh,"info",s);
      return TRUE;
    }
    return FALSE;
  }

  // try help for a library
  int ls = strlen(s);
  char* str = NULL;
  // check that it ends with "[.,_]lib"
  if (strlen(s) >=4 &&  strcmp(&s[ls-3], "lib") == 0)
  {
    if (s[ls - 4] == '.') str = s;
    else
    {
      str = omStrDup(s);
      str[ls - 4] = '.';
    }
  }
  else
  {
    return FALSE;
  }

  char libnamebuf[1024];
  FILE *fp=NULL;
  // first, search for library of that name
  if ((str[1]!='\0') &&
      ((iiLocateLib(str, libnamebuf) && (fp=feFopen(libnamebuf, "rb")) !=NULL)
       ||
       ((fp=feFopen(str,"rb", libnamebuf))!=NULL)))
  {
    EXTERN_VAR FILE *yylpin;
    lib_style_types lib_style; // = OLD_LIBSTYLE;

    yylpin = fp;
    yylplex(str, libnamebuf, &lib_style, IDROOT, FALSE, GET_INFO);
    reinit_yylp();
    if(lib_style == OLD_LIBSTYLE)
    {
      char buf[256];
      fseek(fp, 0, SEEK_SET);
      Warn( "library %s has an old format. Please fix it for the next time",
            str);
      if (str != s) omFree(str);
      BOOLEAN found=FALSE;
      while (fgets( buf, sizeof(buf), fp))
      {
        if (strncmp(buf,"//",2)==0)
        {
          if (found) return TRUE;
        }
        else if ((strncmp(buf,"proc ",5)==0)||(strncmp(buf,"LIB ",4)==0))
        {
          if (!found) WarnS("no help part in library found");
          return TRUE;
        }
        else
        {
          found=TRUE;
          PrintS(buf);
        }
      }
    }
    else
    {
      if (str != s) omFree(str);
      fclose( yylpin );
      PrintS(text_buffer);
      omFree(text_buffer);
      text_buffer=NULL;
    }
    return TRUE;
  }

  if (str != s) omFree(str);
  return FALSE;
}

static long heKeyChksum(char* key)
{
  if (key == NULL || *key == '\0') return 0;
  idhdl h=IDROOT->get(key,myynest);
  if ((h!=NULL) && (IDTYP(h)==PROC_CMD))
  {
    procinfo *pi = IDPROC(h);
    if (pi != NULL) return pi->data.s.help_chksum;
  }
  return 0;
}

/*****************************************************************
 *
 * Implementation : Help Browsers
 *
 *****************************************************************/

STATIC_VAR BOOLEAN feHelpCalled = FALSE;

static void heBrowserHelp(heEntry hentry)
{
  // check checksums of procs
  int kchksum = (hentry != NULL && hentry->chksum > 0 ?
                 heKeyChksum(hentry->key) : 0);
  if (kchksum  && kchksum != hentry->chksum && heOnlineHelp(hentry->key))
    return;

  if (heCurrentHelpBrowser == NULL) feHelpBrowser(NULL, 0);
  assume(heCurrentHelpBrowser != NULL);
  if (! feHelpCalled)
  {
    Warn("Displaying help in browser '%s'.", heCurrentHelpBrowser->browser);
    //if (strcmp(heCurrentHelpBrowser->browser, "netscape") == 0 &&
    //    feResource('h', 0) == NULL)
    //{
    //  Warn("Using URL '%s'.", feResource('u', 0));
    //}
    WarnS("Use 'system(\"--browser\", <browser>);' to change browser,");
    StringSetS("where <browser> can be: ");
    int i = 0;
    i = 0;
    while (heHelpBrowsers[i].browser != NULL)
    {
      if (heHelpBrowsers[i].init_proc(0,i))
        StringAppend("\"%s\", ", heHelpBrowsers[i].browser);
      i++;
    }
    char *browsers=StringEndS();
    if (browsers[strlen(browsers)-2] == ',')
    {
      browsers[strlen(browsers)-2] = '.';
      browsers[strlen(browsers)-1] = '\0';
    }
    WarnS(browsers);
    omFree(browsers);
  }

  heCurrentHelpBrowser->help_proc(hentry, heCurrentHelpBrowserIndex);
  feHelpCalled = TRUE;
}

#define MAX_SYSCMD_LEN MAXPATHLEN*2
static BOOLEAN heGenInit(int warn, int br)
{
  if (heHelpBrowsers[br].required==NULL) return TRUE;
  const char *p=heHelpBrowsers[br].required;
  while (*p>'\0')
  {
    switch (*p)
    {
      case '#': break;
      case ' ': break;
      case 'i': /* singular.hlp */
      case 'x': /* singular.idx */
      case 'h': /* html dir */
               if (feResource(*p, warn) == NULL)
               {
                 if (warn) Warn("resource `%c` not found",*p);
                 return FALSE;
               }
               break;
      case 'D': /* DISPLAY */
               if (getenv("DISPLAY") == NULL)
               {
                 if (warn) WarnS("resource `D` not found");
                 return FALSE;
               }
               break;
      case 'E': /* executable: E:xterm: */
      case 'O': /* OS: O:ix86Mac-darwin/ppcMac-darwin: */
               {
                 char name[128];
                 char exec[128];
                 char op=*p;
                 memset(name,0,128);
                 int i=0;
                 p++;
                 while (((*p==':')||(*p<=' ')) && (*p!='\0')) p++;
                 while((i<127) && (*p>' ') && (*p!=':'))
                 {
                   name[i]=*p; p++; i++;
                 }
                 if (i==0) return FALSE;

                 if ((op=='O') && (strcmp(name,S_UNAME)!=0))
                   return FALSE;
                 if ((op=='E') && (omFindExec(name,exec)==NULL))
                 {
                   if (warn) Warn("executable `%s` not found",name);
                   return FALSE;
                 }
               }
               break;
      default: Warn("unknown char %c",*p);
               break;
    }
    p++;
  }
  return TRUE;
}

static void heGenHelp(heEntry hentry, int br)
{
  char sys[MAX_SYSCMD_LEN];
  const char *p=heHelpBrowsers[br].action;
  if (p==NULL) {PrintS("no action ?\n"); return;}
  memset(sys,0,MAX_SYSCMD_LEN);
  int i=0;
  while ((*p>'\0')&& (i<MAX_SYSCMD_LEN))
  {
    if ((*p)=='%')
    {
      p++;
      switch (*p)
      {
        case 'f': /* local html:file */
        case 'h': /* local html:URL */
        case 'H': /* www html */
                 {
                   char temp[256];
                   char *htmldir = feResource('h' /*"HtmlDir"*/);
                   if ((*p=='h')&&(htmldir!=NULL))
                     strcat(sys,"file://localhost");
                   else if ((*p=='H')||(htmldir==NULL))
                     htmldir = feResource('u' /* %H -> "ManualUrl"*/);
                     /* always defined */
                   if (hentry != NULL && *(hentry->url) != '\0')
                   #ifdef HAVE_VSNPRINTF
                   {
                     if (*p=='H')
                     #ifdef SINGULAR_4_2
                       snprintf(temp,256,"%s/%d-%d/%s", htmldir,
                                  SINGULAR_VERSION/1000,
                                 (SINGULAR_VERSION % 1000)/100,
                     #else
                       snprintf(temp,256,"%s/%d-%d-%d/%s", htmldir,
                                  SINGULAR_VERSION/1000,
                                 (SINGULAR_VERSION % 1000)/100,
                                 (SINGULAR_VERSION % 100)/10,
                     #endif
                       hentry->url);
                     else
                       snprintf(temp,256,"%s/%s", htmldir, hentry->url);
                   }
                   else
                   {
                     if (*p=='H')
                       snprintf(temp,256,"%s/%d-%d-%d/index.htm", htmldir,
                                  SINGULAR_VERSION/1000,
                                 (SINGULAR_VERSION % 1000)/100,
                                 (SINGULAR_VERSION % 100)/10
                       );
                     else
                       snprintf(temp,256,"%s/index.htm", htmldir);
                   }
                   #else
                   {
                     if (*p=='H')
                       sprintf(temp,"%s/%d-%d-%d/%s", htmldir,
                                  SINGULAR_VERSION/1000,
                                 (SINGULAR_VERSION % 1000)/100,
                                 (SINGULAR_VERSION % 100)/10,
                       hentry->url);
                     else
                       sprintf(temp,"%s/%d-%d-%d/%s", htmldir, hentry->url);
                   }
                   else
                     if (*p=='H')
                       sprintf(temp,"%s/%d-%d-%d/index.htm", htmldir,
                                  SINGULAR_VERSION/1000,
                                 (SINGULAR_VERSION % 1000)/100,
                                 (SINGULAR_VERSION % 100)/10
                       );
                     else
                       sprintf(temp,"%s/index.htm", htmldir);
                   }
                   #endif
                   strcat(sys,temp);
                   if ((*p)=='f')
                   { // remove #SEC
                     char *pp=(char *)strchr(sys,'#');
                     if (pp!=NULL)
                     {
                       *pp='\0';
                       i=strlen(sys);
                       memset(pp,0,MAX_SYSCMD_LEN-i);
                     }
                   }
                   i=strlen(sys);
                   break;
                 }
        case 'i': /* singular.hlp */
                 {
                   char *i_res=feResource('i');
                   if (i_res!=NULL) strcat(sys,i_res);
                   else
                   {
                     WarnS("singular.hlp not found");
                     return;
                   }
                   i=strlen(sys);
                   break;
                 }
        case 'n': /* info node */
                 {
                   char temp[256];
                   if ((hentry!=NULL) && (*(hentry->node) != '\0'))
                     sprintf(temp,"%s",hentry->node);
                   //else if ((hentry!=NULL) && (hentry->key!=NULL))
                   //  sprintf(temp,"Index '%s'",hentry->key);
                   else
                     sprintf(temp,"Top");
                   strcat(sys,temp);
                   i=strlen(sys);
                   break;
                 }
        case 'v': /* version number*/
                 {
                   char temp[256];
                   sprintf(temp,"%d-%d-%d",SINGULAR_VERSION/1000,
                                 (SINGULAR_VERSION % 1000)/100,
                                 (SINGULAR_VERSION % 100)/10);
                   strcat(sys,temp);
                   i=strlen(sys);
                   break;
                 }
        default: break;
      }
      p++;
    }
    else
    {
      sys[i]=*p;
      p++;i++;
    }
  }
  Print("running `%s`\n",sys);
  (void) system(sys);
}

static BOOLEAN heDummyInit(int /*warn*/, int /*br*/)
{
  return TRUE;
}
static void heDummyHelp(heEntry /*hentry*/, int /*br*/)
{
  WerrorS("No functioning help browser available.");
}

static BOOLEAN heEmacsInit(int /*warn*/, int /*br*/)
{
  return TRUE;
}
static void heEmacsHelp(heEntry hentry, int /*br*/)
{
  WarnS("Your help command could not be executed. Use");
  Warn("C-h C-s %s",
       (hentry != NULL && *(hentry->node) != '\0' ? hentry->node : "Top"));
  WarnS("to enter the Singular online help. For general");
  WarnS("information on Singular running under Emacs, type C-h m.");
}
static int singular_manual(char *str, BOOLEAN isIndexEntry);
static void heBuiltinHelp(heEntry hentry, int /*br*/)
{
  char* node = omStrDup(hentry != NULL && *(hentry->key) != '\0' ?
                       hentry->key : "Top");
  singular_manual(node,(hentry != NULL) && *(hentry->url)!='\0');
  omFree(node);
}


/* ========================================================================== */
// old, stupid builtin_help
// This could be implemented much more clever, but I'm too lazy to do this now
//
#define HELP_OK        0
#define FIN_INDEX    '\037'
#define HELP_NOT_OPEN  1
#define HELP_NOT_FOUND 2
#define BUF_LEN        256
#define IDX_LEN        256

static inline char tolow(char p)
{
  if (('A'<=p)&&(p<='Z')) return p | 040;
  return p;
}

/*************************************************/
static int show(unsigned long offset, char *close)
{ char buffer[BUF_LEN+1];
  int  lines = 0;
  FILE * help;

  if( (help = fopen(feResource('i'), "rb")) == NULL)
    return HELP_NOT_OPEN;

  fseek(help,  (long)(offset+1), (int)0);
  while( (!feof(help))
        && (*fgets(buffer, BUF_LEN, help) != EOF)
        && (buffer[0] != FIN_INDEX))
  {
    printf("%s", buffer);
    if(lines++==pagelength)
    {
      printf("\n Press <RETURN> to continue or x to exit help.\n");
      fflush(stdout);
      *close = (char)getchar();
      if(*close=='x')
      {
        getchar();
        break;
      }
      lines=0;
    }
  }
  if((*close!='x')&&(pagelength>0))
  {
    printf("\nEnd of part. Press <RETURN> to continue or x to exit help.\n");
    fflush(stdout);
    *close = (char)getchar();
    if(*close=='x')
      getchar();
  }
  fclose(help);
  return HELP_OK;
}

/*************************************************/
static int singular_manual(char *str, BOOLEAN isIndexEntry)
{ FILE *index=NULL;
  unsigned long offset;
  char *p,close=' ';
  int done = 0;
  char buffer[BUF_LEN+1],
       Index[IDX_LEN+1],
       String[IDX_LEN+1];
  Print("HELP >>%s>>\n",str);

  if( (index = fopen(feResource('i'), "rb")) == NULL)
  {
    return HELP_NOT_OPEN;
  }

  if (!isIndexEntry)
  {
    for(p=str; *p; p++) *p = tolow(*p);/* */
    do
    {
      p--;
    }
    while ((p != str) && (*p<=' '));
    p++;
    *p='\0';
    (void)sprintf(String, " %s ", str);
  }
  else
  {
    (void)sprintf(String, " %s", str);
  }

  while(!feof(index)
        && (fgets(buffer, BUF_LEN, index) != (char *)0)
        && (buffer[0] != FIN_INDEX));

  while(!feof(index))
  {
    if (fgets(buffer, BUF_LEN, index)==NULL) break; /*fill buffer */
    if (si_sscanf(buffer, "Node:%[^\177]\177%ld\n", Index, &offset)!=2)
      continue;
    if (!isIndexEntry)
    {
      for(p=Index; *p; p++) *p = tolow(*p);/* */
      (void)strcat(Index, " ");
      if( strstr(Index, String)!=NULL)
      {
        done++; (void)show(offset, &close);
      }
    }
    else if( strcmp(Index, String)==0)
    {
      done++; (void)show(offset, &close);
      break;
    }
    Index[0]='\0';
    if(close=='x')
    break;
  }
  if (index != NULL) (void)fclose(index);
  if(done==0)
  {
    Warn("`%s` not found",String);
    return HELP_NOT_FOUND;
  }
  return HELP_OK;
}
/*************************************************/
