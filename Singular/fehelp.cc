/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: help system
*/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "mod2.h"
#include <mylimits.h>
#include "tok.h"
#include "omalloc.h"
#include "febase.h"
#include "ipid.h"
#include "ipshell.h"
#include "libparse.h"
#include "feOpt.h"
#include "dError.h"

/*****************************************************************
 *
 * Declarations: Data  structures
 *
 *****************************************************************/
#define MAX_HE_ENTRY_LENGTH 60
typedef struct
{
  char key[MAX_HE_ENTRY_LENGTH];
  char node[MAX_HE_ENTRY_LENGTH];
  char url[MAX_HE_ENTRY_LENGTH];
  long  chksum;
} heEntry_s;
typedef  heEntry_s * heEntry;

typedef void (*heBrowserHelpProc)(heEntry hentry);
typedef BOOLEAN (*heBrowserInitProc)(int warn);

typedef struct
{
  char* browser;
  heBrowserInitProc init_proc;
  heBrowserHelpProc help_proc;
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
static BOOLEAN heInfoInit(int);    static void heInfoHelp(heEntry hentry);
#if ! defined(macintosh)
static BOOLEAN heNetscapeInit(int);static void heNetscapeHelp(heEntry hentry);
static BOOLEAN heXinfoInit(int);   static void heXinfoHelp(heEntry hentry);
static BOOLEAN heTkinfoInit(int);  static void heTkinfoHelp(heEntry hentry);
#endif
static BOOLEAN heBuiltinInit(int); static void heBuiltinHelp(heEntry hentry);
static BOOLEAN heDummyInit(int);   static void heDummyHelp(heEntry hentry);
static BOOLEAN heEmacsInit(int);   static void heEmacsHelp(heEntry hentry);

#ifdef ix86_Win
static void heHtmlHelp(heEntry hentry);
static void heWinHelp(heEntry hentry);
#include "sing_win.h"
#endif

static heBrowser heCurrentHelpBrowser = NULL;


/*****************************************************************
 *
 * Definition: available help browsers
 *
 *****************************************************************/
// order is improtant -- first possible help is choosen
static heBrowser_s heHelpBrowsers[] =
{
#ifdef ix86_Win
  { "html",     heDummyInit,    heHtmlHelp},
  { "winhlp",   heDummyInit,    heWinHelp},
#endif
#if ! defined(macintosh)
  { "netscape", heNetscapeInit, heNetscapeHelp},
  { "tkinfo",   heTkinfoInit,   heTkinfoHelp},
  { "xinfo",    heXinfoInit,    heXinfoHelp},
#endif
  { "info",     heInfoInit,     heInfoHelp},
  { "builtin",  heBuiltinInit,  heBuiltinHelp},
  { "dummy",    heDummyInit,    heDummyHelp},
  { "emacs",    heEmacsInit,    heEmacsHelp},
  { NULL, NULL, NULL} // must be the last record
};

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
  char* idxfile = feResource('x' /*"IdxFile"*/);

  // Try exact match of help string with key in index
  if (!key_is_regexp && idxfile != NULL && heKey2Entry(idxfile, str, &hentry))
  {
    heBrowserHelp(&hentry);
    return;
  }

  // try proc help and library help
  if (! key_is_regexp && heOnlineHelp(str)) return;

  // Try to match approximately with key in index file
  if (idxfile != NULL)
  {
    char* matches = StringSetS("");
    int found = heReKey2Entry(idxfile, str, &hentry);

    // Try to match with str*
    if (found == 0)
    {
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
    Warn("Try one of");
    PrintS(matches);
    PrintS("\n");
    return;
  }

  // no idx file, let Browsers deal with it, if they can
  strcpy(hentry.key, str);
  *hentry.node = '\0';
  *hentry.url = '\0';
  hentry.chksum = 0;
  heBrowserHelp(&hentry);
}

char* feHelpBrowser(char* which, int warn)
{
  int i = 0;

  // if no argument, choose first available help browser
  if (which == NULL || *which == '\0')
  {
    // return, if already set
    if (heCurrentHelpBrowser != NULL)
      return heCurrentHelpBrowser->browser;

    // First, try emacs, if emacs-option is set
    // Under Win, always use html
#ifndef ix86_Win
    if (feOptValue(FE_OPT_EMACS) != NULL)
    {
      while (heHelpBrowsers[i].browser != NULL)
      {
        if (strcmp(heHelpBrowsers[i].browser, "emacs") == 0 &&
            (heHelpBrowsers[i].init_proc(0)))
        {
          heCurrentHelpBrowser = &(heHelpBrowsers[i]);
          goto Finish;
        }
        i++;
      }
      i=0;
    }
#endif
    while (heHelpBrowsers[i].browser != NULL)
    {
      if (heHelpBrowsers[i].init_proc(0))
      {
        heCurrentHelpBrowser = &(heHelpBrowsers[i]);
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
    if (heHelpBrowsers[i].init_proc(warn))
    {
      heCurrentHelpBrowser = &(heHelpBrowsers[i]);
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
  while (heHelpBrowsers[i].browser != NULL)
  {
    if (heHelpBrowsers[i].init_proc(warn))
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
  while (*s <= ' ' && *s != '\0') s++;
  if (*s == '\0') return NULL;
  char *ss=s;
  while (*ss!='\0') ss++;
  ss--;
  while (*ss <= ' ' && *ss != '\0')
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
  char c, k;
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

        // get url
        hentry->node[i] = '\0';
        i = 0;
        while ((c = getc(fd)) != '\t' && c != EOF)
        {
          hentry->url[i] = c;
          i++;
        }
        if (c == EOF) goto Failure;

        // get chksum
        hentry->url[i] = '\0';

        if (fscanf(fd, "%ld\n", &(hentry->chksum)) != 1)
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
  while (fscanf(fd, "%[^\t]\t%*[^\n]\n", index_key) == 1)
  {
    if (strmatch(index_key, key))
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

// try to find the help string as a loaded procedure or library
// if found, display the help and return TRUE
// otherwise, return FALSE
static BOOLEAN heOnlineHelp(char* s)
{
#ifdef HAVE_NAMESPACES
  idhdl h, ns;
  iiname2hdl(s, &ns, &h);
#else /* HAVE_NAMESPACES */
  idhdl h=IDROOT->get(s,myynest);
#endif /* HAVE_NAMESPACES */

  // try help for a procedure
  if ((h!=NULL) && (IDTYP(h)==PROC_CMD))
  {
    char *lib=iiGetLibName(IDPROC(h));
    if((lib!=NULL)&&(*lib!='\0'))
    {
      Print("// proc %s from lib %s\n",s,lib);
      s=iiGetLibProcBuffer(IDPROC(h), 0);
      if (s!=NULL)
      {
        PrintS(s);
        omFree((ADDRESS)s);
      }
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

  char libnamebuf[128];
  FILE *fp=NULL;
  // first, search for library of that name in LIB string
  if ((str[1]!='\0') &&
      ((iiLocateLib(str, libnamebuf) && (fp=feFopen(libnamebuf, "rb")) !=NULL)
       ||
       ((fp=feFopen(str,"rb", libnamebuf))!=NULL)))
  {
    extern FILE *yylpin;
    lib_style_types lib_style; // = OLD_LIBSTYLE;

    yylpin = fp;
#ifdef HAVE_NAMESPACES
    yylplex(str, libnamebuf, &lib_style, IDROOT, FALSE, GET_INFO);
#else /* HAVE_NAMESPACES */
#ifdef HAVE_NS
    yylplex(str, libnamebuf, &lib_style, IDROOT, FALSE, GET_INFO);
#else /* HAVE_NS */
    yylplex(str, libnamebuf, &lib_style, GET_INFO);
#endif /* HAVE_NS */
#endif /* HAVE_NAMESPACES */
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
#ifdef HAVE_NAMESPACES
  idhdl h, ns;
  iiname2hdl(key, &ns, &h);
#else /* HAVE_NAMESPACES */
  idhdl h=IDROOT->get(key,myynest);
#endif /* HAVE_NAMESPACES */
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

static BOOLEAN feHelpCalled = FALSE;

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
    if (strcmp(heCurrentHelpBrowser->browser, "netscape") == 0 &&
        feResource('h', 0) == NULL)
    {
      Warn("Using URL '%s'.", feResource('u', 0));
    }
    Warn("Use 'system(\"--browser\", <browser>);' to change browser,");
    char* browsers = StringSetS("where <browser> can be: ");
    int i = 0;
    i = 0;
    while (heHelpBrowsers[i].browser != NULL)
    {
      if (heHelpBrowsers[i].init_proc(0))
        StringAppend("\"%s\", ", heHelpBrowsers[i].browser);
      i++;
    }
    if (browsers[strlen(browsers)-2] == ',')
    {
      browsers[strlen(browsers)-2] = '.';
      browsers[strlen(browsers)-1] = '\0';
    }
    WarnS(browsers);
  }

  heCurrentHelpBrowser->help_proc(hentry);
  feHelpCalled = TRUE;
}

#define MAX_SYSCMD_LEN MAXPATHLEN*2
// browser functions
static BOOLEAN heInfoInit(int warn)
{
  if (feResource('i', warn) == NULL)
  {
    if (warn) WarnS("'info' help browser not available: no InfoFile");
    return FALSE;
  }
  if (feResource('I') == NULL)
  {
    if (warn)
      WarnS("'info' help browser not available: no 'info' program found");
    return FALSE;
  }
  return TRUE;
}
static void heInfoHelp(heEntry hentry)
{
  char sys[MAX_SYSCMD_LEN];

  if (hentry != NULL && *(hentry->key) != '\0')
  {
    if (*(hentry->node) != '\0')
      sprintf(sys, "%s -f %s --node='%s'",
              feResource('I'), feResource('i'), hentry->node);
    else
      sprintf(sys, "%s -f %s Index '%s'",
              feResource('I'), feResource('i'), hentry->key);
  }
  else
    sprintf(sys, "%s -f %s --node=Top", feResource('I'), feResource('i'));
  system(sys);
}

#if ! defined(macintosh)
static BOOLEAN heNetscapeInit(int warn)
{
  if (feResource('N' /*"netscape"*/, warn) == NULL)
  {
    if (warn) WarnS("'netscape' help browser not available: no 'netscape' program found");
    return FALSE;
  }
#ifndef ix86_Win
  if (getenv("DISPLAY") == NULL)
  {
    if (warn) WarnS("'netscape' help browser not available:");
    if (warn) WarnS("Environment variable DISPLAY not set");
    return FALSE;
  }
#endif

  if (feResource('h' /*"HtmlDir"*/, (feOptValue(FE_OPT_ALLOW_NET)? 0 : warn))
      == NULL)
  {
    if (warn) WarnS("No local HtmlDir found.");
    if (feOptValue(FE_OPT_ALLOW_NET))
    {
      if (warn) Warn("Using URL '%s' instead.", feResource('u' /*"ManualUrl"*/, warn));
    }
    else
      return FALSE;
  }
  return TRUE;
}

static void heNetscapeHelp(heEntry hentry)
{
  char sys[MAX_SYSCMD_LEN];
  char url[MAXPATHLEN];
  char* htmldir = feResource('h' /*"HtmlDir"*/);
  char* urltype;

  if (htmldir == NULL)
  {
    urltype = "";
    htmldir = feResource('u' /*"ManualUrl"*/);
  }
  else
  {
    urltype = "file:";
  }

  if (hentry != NULL && *(hentry->url) != '\0')
  {
    sprintf(url, "%s%s/%s", urltype, htmldir, hentry->url);
  }
  else
  {
    sprintf(url, "%s%s/index.htm", urltype, htmldir);
  }
#ifndef ix86_Win
  sprintf(sys, "%s --remote 'OpenUrl(%s)' > /dev/null 2>&1",
#else
  sprintf(sys, "%s %s",
#endif
          feResource('N' /*"netscape"*/), url);

  // --remote exits with status != 0 if netscaep isn't already running
  if (system(sys) != 0)
  {
    sprintf(sys, "%s %s &", feResource('N' /*"netscape"*/), url);
    system(sys);
  }
  else
  {
    if (! feHelpCalled)
    {
      Warn("Help is displayed in already running 'netscape'.");
    }
  }
}

#ifdef ix86_Win
static void heHtmlHelp(heEntry hentry)
{
  char url[MAXPATHLEN];
  char* html_dir = feResource('h' /*"HtmlDir"*/);
  sprintf(url, "%s/%s",
          (html_dir != NULL ? html_dir : feResource('u' /*"ManualUrl"*/)),
          (hentry!=NULL && *(hentry->url)!='\0' ? hentry->url : "index.htm"));

  heOpenWinntUrl(url, (html_dir != NULL ? 1 : 0));
}

static void heWinHelp(heEntry hentry)
{
  char keyw[MAX_HE_ENTRY_LENGTH];
  if ((hentry!=NULL)&&(hentry->key!=NULL))
    strcpy(keyw,hentry->key);
  else
    strcpy(keyw," ");
  char* helppath = feResource('h' /*"HtmlDir"*/);
  const char *filename="/Manual.hlp";
  int helppath_len=0;
  if (helppath!=NULL) helppath_len=strlen(helppath);
  char *callpath=(char *)omAlloc0(helppath_len+strlen(filename)+1);
  if ((helppath!=NULL) && (*helppath>' '))
    strcpy(callpath,helppath);
  strcat(callpath,filename);
  heOpenWinntHlp(keyw,callpath);
  omfree(callpath);
}
#endif

static BOOLEAN heXinfoInit(int warn)
{
#ifndef ix86_Win
  if (getenv("DISPLAY") == NULL)
  {
    if (warn) WarnS("'xinfo' help browser not available:");
    if (warn) WarnS("Environment variable DISPLAY not set");
    return FALSE;
  }
#endif
  if (feResource('i', warn) == NULL)
  {
    if (warn) WarnS("'xinfo' help browser not available: no InfoFile");
    return FALSE;
  }
  if (feResource('I', warn) == NULL)
  {
    if (warn) WarnS("'xinfo' help browser not available: no 'info' program found");
    return FALSE;
  }
  if (feResource('X', warn) == NULL)
  {
    if (warn) WarnS("'xinfo' help browser not available: no 'xterm' program found");
    return FALSE;
  }
  return TRUE;
}
static void heXinfoHelp(heEntry hentry)
{
  char sys[MAX_SYSCMD_LEN];

#ifdef ix86_Win
#define EXTRA_XTERM_ARGS "+vb -sb -fb Courier-bold-12 -tn linux -cr Red3"
#else
#define EXTRA_XTERM_ARGS ""
#endif

  if (hentry != NULL && *(hentry->key) != '\0')
  {
    if (*(hentry->node) != '\0')
      sprintf(sys, "%s %s -e %s -f %s --node='%s' &",
              feResource('X'), EXTRA_XTERM_ARGS,
              feResource('I'), feResource('i'), hentry->node);
    else
      sprintf(sys, "%s %s -e %s -f %s Index '%s' &",
              feResource('X'), EXTRA_XTERM_ARGS,
              feResource('I'), feResource('i'), hentry->key);
  }
  else
    sprintf(sys, "%s %s -e %s -f %s --node=Top &",
            feResource('X'), EXTRA_XTERM_ARGS,
            feResource('I'), feResource('i'));
  system(sys);
}

static BOOLEAN heTkinfoInit(int warn)
{
#ifndef ix86_Win
  if (getenv("DISPLAY") == NULL)
  {
    if (warn) WarnS("'tkinfo' help browser not available:");
    if (warn) WarnS("Environment variable DISPLAY not set");
    return FALSE;
  }
#endif
  if (feResource('i', warn) == NULL)
  {
    if (warn) WarnS("'tkinfo' help browser not available: no InfoFile");
    return FALSE;
  }
  if (feResource('T', warn) == NULL)
  {
    if (warn) WarnS("'tkinfo' help browser not available: no 'tkinfo' program found");
    return FALSE;
  }
  return TRUE;
}
static void heTkinfoHelp(heEntry hentry)
{
  char sys[MAX_SYSCMD_LEN];
  if (hentry != NULL && *(hentry->node) != '\0')
  {
    sprintf(sys, "%s '(%s)%s' &",
            feResource('T'), feResource('i'), hentry->node);
  }
  else
  {
    sprintf(sys, "%s %s &",
            feResource('T'), feResource('i'));
  }
  system(sys);
}
#endif // ! defined(macintosh)

static BOOLEAN heDummyInit(int warn)
{
  return TRUE;
}
static void heDummyHelp(heEntry hentry)
{
  Werror("No functioning help browser available.");
}

static BOOLEAN heEmacsInit(int warn)
{
  return TRUE;
}
static void heEmacsHelp(heEntry hentry)
{
  WarnS("Your help command could not be executed. Use");
  Warn("C-h C-s %s",
       (hentry != NULL && *(hentry->node) != '\0' ? hentry->node : "Top"));
  Warn("to enter the Singular online help. For general");
  Warn("information on Singular running under Emacs, type C-h m.");
}
static BOOLEAN heBuiltinInit(int warn)
{
  if (feResource('i', warn) == NULL)
  {
    if (warn) WarnS("'builtin' help browser not available: no InfoFile");
    return FALSE;
  }
  return TRUE;
}
static int singular_manual(char *str);
static void heBuiltinHelp(heEntry hentry)
{
  char* node = omStrDup(hentry != NULL && *(hentry->node) != '\0' ?
                       hentry->node : "Top");
  singular_manual(node);
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
#define IDX_LEN        64
#define MAX_LINES      21

#ifdef macintosh
static char tolow(char p)
#else
static inline char tolow(char p)
#endif
{
  if (('A'<=p)&&(p<='Z')) return p | 040;
  return p;
}

/*************************************************/
static int show(unsigned long offset,FILE *help, char *close)
{ char buffer[BUF_LEN+1];
  int  lines = 0;

  if( help== NULL)
    if( (help = fopen(feResource('i'), "rb")) == NULL)
      return HELP_NOT_OPEN;

  fseek(help,  (long)(offset+1), (int)0);
  while( !feof(help)
        && *fgets(buffer, BUF_LEN, help) != EOF
        && buffer[0] != FIN_INDEX)
  {
    printf("%s", buffer);
    if(lines++> MAX_LINES)
    {
#ifdef macintosh
      printf("\n Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
      printf("\n Press <RETURN> to continue or x to exit help.\n");
#endif
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
  if(*close!='x')
  {
#ifdef macintosh
    printf("\nEnd of part. Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
    printf("\nEnd of part. Press <RETURN> to continue or x to exit help.\n");
#endif
    fflush(stdout);
    *close = (char)getchar();
    if(*close=='x')
      getchar();
  }
  return HELP_OK;
}

/*************************************************/
static int singular_manual(char *str)
{ FILE *index=NULL,*help=NULL;
  unsigned long offset;
  char *p,close;
  int done = 0;
  char buffer[BUF_LEN+1],
       Index[IDX_LEN+1],
       String[IDX_LEN+1];

  if( (index = fopen(feResource('i'), "rb")) == NULL)
  {
    return HELP_NOT_OPEN;
  }

  for(p=str; *p; p++) *p = tolow(*p);/* */
  do
  {
    p--;
  }
  while ((p != str) && (*p<=' '));
  p++;
  *p='\0';
  (void)sprintf(String, " %s ", str);

  while(!feof(index)
        && fgets(buffer, BUF_LEN, index) != (char *)0
        && buffer[0] != FIN_INDEX);

  while(!feof(index))
  {
    (void)fgets(buffer, BUF_LEN, index); /* */
    (void)sscanf(buffer, "Node:%[^\177]\177%ld\n", Index, &offset);
    for(p=Index; *p; p++) *p = tolow(*p);/* */
    (void)strcat(Index, " ");
    if( strstr(Index, String)!=NULL)
    {
      done++; (void)show(offset, help, &close);
    }
    Index[0]='\0';
    if(close=='x')
    break;
  }
  if (index != NULL) (void)fclose(index);
  if (help != NULL) (void)fclose(help);
  if(! done)
  {
    Warn("`%s` not found",String);
    return HELP_NOT_FOUND;
  }
  return HELP_OK;
}
/*************************************************/
