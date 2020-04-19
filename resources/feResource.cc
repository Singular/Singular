/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: management of resources
*/

#include "singular_resourcesconfig.h"
#include "feResource.h"
#include "omFindExec.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/param.h>


VAR char* feArgv0 = NULL;

#ifdef AIX_4
#ifndef HAVE_PUTENV
#define HAVE_PUTENV 1
#endif
#endif

#if defined(HPUX_10) || defined(HPUX_9)
#ifndef HAVE_SETENV
extern "C" int setenv(const char *name, const char *value, int overwrite);
#endif
#endif


//char* feResource(const char id, int warn = -1);
//char* feResource(const char* key, int warn = -1);

// define RESOURCE_DEBUG for chattering about resource management
// #define RESOURCE_DEBUG

#define SINGULAR_DEFAULT_DIR PREFIX

/*****************************************************************
 *
 * Declarations: Data  structures
 *
 *****************************************************************/
// feSprintf transforms format strings as follows:
// 1.) substrings of the form %c (c being a letter) are replaced by respective resource value
// 2.) substrings of the form $string are replaced by value of resp. env variable

// feCleanResource makes furthermore  the following transformations (except for URL resources)
// 1.) '/' characters are replaced by respective directory - separators
// 2.) ';' characters are replaced by respective path separators
VAR feResourceConfig_s feResourceConfigs[] =
{
  {"SearchPath",    's', feResPath,  NULL,
   "$SINGULARPATH;"
   "%D/singular/LIB;"
   "%r/share/singular/LIB;"
   "%b/../share/singular/LIB;"
   // gftables:
   "%D/factory;"
   "%r/share/factory;"
   "%b/LIB;"
   "%b/../factory;"
   // path for dynamic modules, should match ProcDir:
   "%b/MOD;"
   "%r/lib/singular/MOD;"
   "%r/libexec/singular/MOD;"
   LIB_DIR "/singular/MOD;"
   LIBEXEC_DIR "/singular/MOD;"
   "%b",
   (char *)""},
  {"Singular",  'S',    feResBinary,"SINGULAR_EXECUTABLE",  "%d/Singular",          (char *)""},
  {"BinDir",    'b',    feResDir,   "SINGULAR_BIN_DIR",     "",                  (char *)""},
  // should be changed to %b/../lib/singular/pProcs/:
  {"ProcDir",   'P',    feResPath,  "SINGULAR_PROCS_DIR",
     "%b/MOD;"
     "%r/lib/singular/MOD;"
     "%r/libexec/singular/MOD;"
     LIB_DIR "/singular/MOD;"   /*debian: -> /usr/lib/singular/MOD */
     LIBEXEC_DIR "/singular/MOD" ,                  (char *)""},
  {"RootDir",   'r',    feResDir,   "SINGULAR_ROOT_DIR",    "%b/..",                (char *)""},
  {"DataDir",   'D',    feResDir,   "SINGULAR_DATA_DIR",    "%b/../share/",          (char *)""},
  {"DefaultDir",'d',    feResDir,   "SINGULAR_DEFAULT_DIR",  SINGULAR_DEFAULT_DIR,  (char *)""},
  {"InfoFile",  'i',    feResFile,  "SINGULAR_INFO_FILE",   "%D/info/singular.hlp", (char *)""},
  {"IdxFile",   'x',    feResFile,  "SINGULAR_IDX_FILE",    "%D/singular/singular.idx",  (char *)""},
  {"HtmlDir",   'h',    feResDir,   "SINGULAR_HTML_DIR",    "%D/singular/html",              (char *)""},
  {"ManualUrl", 'u',    feResUrl,   "SINGULAR_URL",         "https://www.singular.uni-kl.de/Manual/",    (char *)""},
  {"ExDir",     'm',    feResDir,   "SINGULAR_EXAMPLES_DIR","%r/examples",          (char *)""},
  {"Path",      'p',    feResPath,  NULL,                   "%b;%P;$PATH",             (char *)""},

#ifdef __CYGWIN__
  {"emacs",     'E',    feResBinary,"ESINGULAR_EMACS",      "%b/emacs.exe",             (char *)""},
  {"xemacs",    'A',    feResBinary,"ESINGULAR_EMACS",      "%b/xemacs.exe",            (char *)""},
  {"SingularEmacs",'M', feResBinary,"ESINGULAR_SINGULAR",   "%b/Singular.exe",          (char *)""},
#else
  {"emacs",     'E',    feResBinary,"ESINGULAR_EMACS",      "%b/emacs",             (char *)""},
  {"xemacs",    'A',    feResBinary,"ESINGULAR_EMACS",      "%b/xemacs",            (char *)""},
  {"SingularEmacs",'M', feResBinary,"ESINGULAR_SINGULAR",   "%b/Singular",          (char *)""},
#endif
  {"EmacsLoad", 'l',    feResFile,  "ESINGULAR_EMACS_LOAD", "%e/.emacs-singular",   (char *)""},
  {"EmacsDir",  'e',    feResDir,   "ESINGULAR_EMACS_DIR",  "%D/singular/emacs",             (char *)""},
  {"SingularXterm",'M', feResBinary,"TSINGULAR_SINGULAR",   "%b/Singular",          (char *)""},
#ifdef __CYGWIN__
  {"rxvt",      'X',    feResBinary,"RXVT",                 "%b/rxvt",              (char *)""},
#else
  {"xterm",     'X',    feResBinary,"XTERM",                "%b/xterm",             (char *)""},
#endif
  {"EmacsDir",  'e',    feResDir,   "SINGULAR_EMACS_DIR",   "%r/emacs",             (char *)""},
  {NULL, 0, feResUndef, NULL, NULL, NULL}, // must be the last record
};


/*****************************************************************
 *
 * Declarations: Local variables / functions
 *
 *****************************************************************/

#define MAXRESOURCELEN 5*MAXPATHLEN

static feResourceConfig feGetResourceConfig(const char id);
static feResourceConfig feGetResourceConfig(const char* key);
static char* feResource(feResourceConfig config, int warn);
static char* feResourceDefault(feResourceConfig config);
static char* feInitResource(feResourceConfig config, int warn);
static char* feGetExpandedExecutable();
static int feVerifyResourceValue(feResourceType type, char* value);
static char* feCleanResourceValue(feResourceType type, char* value);
static char* feCleanUpFile(char* fname);
static char* feCleanUpPath(char* path);
static void mystrcpy(char* d, char* s);
static char* feSprintf(char* s, const char* fmt, int warn = -1);
#if defined(__CYGWIN__) && defined(__GNUC__)
// utility function of Cygwin32:
extern "C" int cygwin32_posix_path_list_p (const char *path);
#endif

/*****************************************************************
 *
 * Public functions
 *
 *****************************************************************/
char* feResource(const char* key, int warn)
{
  return feResource(feGetResourceConfig(key), warn);
}

char* feResource(const char id, int warn)
{
  return feResource(feGetResourceConfig(id), warn);
}

char* feGetResource(const char id, int warn)
{
  return feResource(feGetResourceConfig(id), warn);
}

char* feResourceDefault(const char id)
{
  return feResourceDefault(feGetResourceConfig(id));
}

char* feResourceDefault(const char* key)
{
  return feResourceDefault(feGetResourceConfig(key));
}

void feInitResources(const char* argv0)
{
  if (argv0==NULL)
  {
    //WarnS("illegal argv[0]==NULL");
    feArgv0 = (char*)malloc(MAXPATHLEN+strlen("/Singular"));
    getcwd(feArgv0, MAXPATHLEN);
    strcat(feArgv0,"/Singular");
  }
  else
    feArgv0 = strdup(argv0);
#ifdef RESOURCE_DEBUG
  printf("feInitResources(argv0: '%s'): entering...\n", feArgv0);
#endif
  // init some Resources
  feResource('b');
  feResource('r');
  // don't complain about stuff when initializing SingularPath
  feResource('s',0);
  feResource('P');

#if defined(HAVE_SETENV) || defined(HAVE_PUTENV)
  char* path = feResource('p');
#ifdef RESOURCE_DEBUG
  printf("feInitResources(argv0): setting path with '%s'\n", path);
#endif
#ifdef HAVE_PUTENV
  if (path != NULL) { char *s=(char *)malloc(strlen(path)+6);
                      sprintf(s,"PATH=%s",path);
                      putenv(s);
                    }
#else
  if (path != NULL) setenv("PATH", path, 1);
#endif
#endif
}

void feReInitResources()
{
  int i = 0;
  while (feResourceConfigs[i].key != NULL)
  {
    if ((feResourceConfigs[i].value != NULL)
    && (feResourceConfigs[i].value[0] != '\0'))
    {
      free(feResourceConfigs[i].value);
      feResourceConfigs[i].value = (char *)"";
    }
    i++;
  }
#ifdef RESOURCE_DEBUG
  printf("feInitResources(): entering...\n");
#endif
  // init some Resources
  feResource('b');
  feResource('r');
  // don't complain about stuff when initializing SingularPath
  feResource('s',0);
}

/*****************************************************************
 *
 * Local functions
 *
 *****************************************************************/
static feResourceConfig feGetResourceConfig(const char id)
{
  int i = 0;
  while (feResourceConfigs[i].key != NULL)
  {
    if (feResourceConfigs[i].id == id) return &(feResourceConfigs[i]);
    i++;
  }
  return NULL;
}

static feResourceConfig feGetResourceConfig(const char* key)
{
  int i = 0;
  while (feResourceConfigs[i].key != NULL)
  {
    if (strcmp(feResourceConfigs[i].key, key) == 0)
      return &(feResourceConfigs[i]);
    i++;
  }
  return NULL;
}

static char* feResource(feResourceConfig config, int warn)
{
  if (config == NULL) return NULL;
  if (config->value != NULL && *(config->value) != '\0') return config->value;
  return feInitResource(config, warn);
}

static char* feResourceDefault(feResourceConfig config)
{
  if (config == NULL) return NULL;
  char* value = (char*) malloc(MAXRESOURCELEN);
  feSprintf(value, config->fmt, -1);
  return value;
}

static char* feInitResource(feResourceConfig config, int warn)
{
  /*assume(config != NULL);*/
#ifdef RESOURCE_DEBUG
  printf("feInitResource(config->key: '%s', warn: '%d') : entering ...\n", config->key, warn);
#endif

  char value[MAXRESOURCELEN];
  // now we have to work
  // First, check Environment variable
  if (config->env != NULL)
  {
    char* evalue = getenv(config->env);
    if (evalue != NULL)
    {
#ifdef RESOURCE_DEBUG
      printf("feInitResource(config,warn): Found value from env:%s\n", evalue);
#endif
      strcpy(value, evalue);
      if (config->type == feResBinary  // do not verify binaries
          ||
          feVerifyResourceValue(config->type,
                                feCleanResourceValue(config->type, value)))
      {
#ifdef RESOURCE_DEBUG
        printf("feInitResource(config,warn): Set value of config (with key: '%s') to '%s'\n", config->key, value);
#endif
        config->value = strdup(value);
        return config->value;
      }
    }
  }

  *value = '\0';
  // Special treatment of executable
  if (config->id == 'S')
  {
    char* executable = feGetExpandedExecutable();
    if (executable != NULL)
    {
#ifdef RESOURCE_DEBUG
      printf("exec:%s\n", executable);
#endif
      strcpy(value, executable);
#ifdef RESOURCE_DEBUG
      printf("value:%s\n", value);
#endif
      free(executable);
    }
  }
  // and bindir
  else if (config->id == 'b')
  {
    char* executable = feResource('S');
#ifdef RESOURCE_DEBUG
      printf("feInitResource(config,warn): Get '%s' from \"%s\"\n", config->key, executable);
#endif
    if (executable != NULL)
    {
      strcpy(value, executable);
      executable = strrchr(value, DIR_SEP);
      if (executable != NULL) *executable = '\0';
    }
  }

#ifdef RESOURCE_DEBUG
  printf("value:%s\n", value);
#endif

  if (*value == '\0' && config->fmt != NULL )
  {
    feSprintf(value, config->fmt, warn);
  }
  else if (config->fmt == NULL)
  {
    printf("Bug >>Wrong Resource Specification of '%s'<< at \"%s:%d\"\n",config->key,__FILE__,__LINE__);
    // TODO: printf -> WarnS???
    return NULL;
  }

  // Clean and verify
  if (feVerifyResourceValue(config->type,
                            feCleanResourceValue(config->type, value)))
  {
#ifdef RESOURCE_DEBUG
    printf("feInitResource(config,warn): Set value of '%s' to \"%s\"\n", config->key, value);
#endif
    config->value = strdup(value);
    return config->value;
  }
  else if (config->type == feResBinary)
  {
    // for binaries, search through PATH once more
    char* executable = omFindExec(config->key, value);
    if (executable != NULL)
    {
      if (feVerifyResourceValue(config->type,
                                feCleanResourceValue(config->type, value)))
      {
        config->value = strdup(value);
#ifdef RESOURCE_DEBUG
        printf("feInitResource(config,warn): Set value of '%s' to \"%s\"\n", config->key, config->value);
#endif
        return config->value;
      }
    }
  }

  // issue warning if explicitely requested, or if
  // this value is gotten for the first time
  if (warn > 0 || (warn < 0 && config->value != NULL))
  {
    printf("// ** Could not get '%s'.\n", config->key);
    printf("// ** Either set environment variable '%s' to '%s',\n",
         config->env, config->key);
    feSprintf(value, config->fmt, warn);
    printf("// ** or make sure that '%s' is at \"%s\"\n", config->key, value);
  }
#ifdef RESOURCE_DEBUG
  printf("feInitResource(config,warn): Set value of '%s' to NULL", config->key);
#endif
  config->value = NULL;
  return NULL;
}

static char* feGetExpandedExecutable()
{
  if (feArgv0 == NULL || *feArgv0 == '\0')
  {
    if (feArgv0 == NULL)
      printf("Bug >>feArgv0 == NULL<< at %s:%d\n",__FILE__,__LINE__);
    else
      printf("Bug >>feArgv0 == ''<< at %s:%d\n",__FILE__,__LINE__);
    return NULL;
  }
#ifdef __CYGWIN__ // stupid WINNT sometimes gives you argv[0] within ""
  if (*feArgv0 == '"')
  {
    int l = strlen(feArgv0);
    if (feArgv0[l-1] == '"')
    {
      feArgv0[l-1] = '\0';
      feArgv0++;
    }
  }
#endif
#ifdef RESOURCE_DEBUG
  printf("feGetExpandedExecutable: calling find_exec with \"%s\"\n", feArgv0);
#endif
  char executable[MAXRESOURCELEN];
  char* value = omFindExec(feArgv0, executable);
#ifdef RESOURCE_DEBUG
  printf("feGetExpandedExecutable: find_exec exited with \"%s\": %d\n", executable, access(executable, X_OK));
#endif
  if (value == NULL)
  {
    printf("Bug >>Could not get expanded executable from \"%s\"<< at %s:%d\n",feArgv0,__FILE__,__LINE__);
    return NULL;
  }
  return strdup(value);
}


static int feVerifyResourceValue(feResourceType type, char* value)
{
#ifdef RESOURCE_DEBUG
  printf("feVerifyResourceValue(type: %d, value: \"%s\"): entering\n", (int)type, value);
  printf("Access: ROK: %d, XOK: %d\n", access(value, R_OK), access(value, X_OK));
#endif
  switch(type)
  {
      case feResUrl:
      case feResPath:
        return 1;

      case feResFile:
        return ! access(value, R_OK);

      case feResBinary:
      case feResDir:
        return ! access(value, X_OK);

      default:
        return 0;
  }
}

/*****************************************************************
 *
 * Cleaning/Transformations of resource values
 *
 *****************************************************************/

static char* feCleanResourceValue(feResourceType type, char* value)
{
  if (value == NULL || *value == '\0') return value;
#ifdef RESOURCE_DEBUG
      printf("Clean value:%s\n", value);
#endif
#ifdef __CYGWIN__
#ifdef RESOURCE_DEBUG
      printf("Clean WINNT value:%s\n", value);
#endif
  if (type == feResBinary)
  {
    int l = strlen(value);
    if (l < 4 || (strcmp(&value[l-4], ".exe") != 0 &&
                  strcmp(&value[l-4], ".EXE") != 0))
      strcat(value, ".exe");
  }
#endif
  if (type == feResFile || type == feResBinary || type == feResDir)
    return feCleanUpFile(value);
  if (type == feResPath)
    return feCleanUpPath(value);
  return value;
}

static char* feCleanUpFile(char* fname)
{
  char* fn;

#ifdef RESOURCE_DEBUG
  printf("feCleanUpFile: entering with =%s=\n", fname);
#endif
  // Remove unnecessary .. and //
  for (fn = fname; *fn != '\0'; fn++)
  {
    if (*fn == '/')
    {
      if (*(fn+1) == '\0')
      {
        if (fname != fn) *fn = '\0';
        break;
      }
      if (*(fn + 1) == '/' && (fname != fn))
      {
        mystrcpy(fn, fn+1);
        fn--;
      }
      else if (*(fn+1) == '.')
      {
        if (*(fn+2) == '.' && (*(fn + 3) == '/' || *(fn + 3) == '\0'))
        {
        #if 0
        // this does not work: ./../../mmm will be changed to ./../mmm
        // but we only want to change ././mmm to ./mmm
          *fn = '\0';
          s = strrchr(fname, '/');
          if (s != NULL)
          {
            mystrcpy(s+1, fn + (*(fn + 3) != '\0' ? 4 : 3));
            fn = s-1;
          }
          else
          {
            *fn = '/';
          }
        #endif
        }
        else if (*(fn+2) == '/' || *(fn+2) == '\0')
        {
          mystrcpy(fn+1, fn+3);
          fn--;
        }
      }
    }
  }

#ifdef RESOURCE_DEBUG
  printf("feCleanUpFile: leaving with =%s=\n", fname);
#endif
  return fname;
}

// remove duplicates dir resp. those which do not exist
static char* feCleanUpPath(char* path)
{
#ifdef RESOURCE_DEBUG
  printf("feCleanUpPath: entering with: =%s=\n", path);
#endif
  if (path == NULL) return path;

  int n_comps = 1, i, j;
  char* opath = path;
  char** path_comps;

  for (; *path != '\0'; path++)
  {
    if (*path == fePathSep) n_comps++;
    else if (*path == ';')
    {
      *path = fePathSep;
      n_comps++;
    }
  }

  path_comps = (char**) malloc(n_comps*sizeof(char*));
  path_comps[0]=opath;
  path=opath;
  i = 1;

  if (i < n_comps)
  {
    while (1)
    {
      if (*path == fePathSep)
      {
        *path = '\0';
        path_comps[i] = path+1;
        i++;
        if (i == n_comps) break;
      }
      path++;
    }
  }

  for (i=0; i<n_comps; i++)
    path_comps[i] = feCleanUpFile(path_comps[i]);
#ifdef RESOURCE_DEBUG
  printf("feCleanUpPath: after CleanUpName: ");
  for (i=0; i<n_comps; i++)
    printf("%s:", path_comps[i]);
  printf("\n");
#endif

  for (i=0; i<n_comps;)
  {
#ifdef RESOURCE_DEBUG
    if (access(path_comps[i], X_OK | R_OK))
      printf("feCleanUpPath: remove %d:%s -- can not access\n", i, path_comps[i]);
#endif
    if ( ! access(path_comps[i], X_OK | R_OK))
    {
      // x- permission is granted -- we assume that it is a dir
      for (j=0; j<i; j++)
      {
        if (strcmp(path_comps[j], path_comps[i]) == 0)
        {
          // found a duplicate
#ifdef RESOURCE_DEBUG
          printf("feCleanUpPath: remove %d:%s -- equal to %d:%s\n", j, path_comps[j], i, path_comps[i]);
#endif
          j = i+1;
          break;
        }
      }
      if (j == i)
      {
        i++;
        continue;
      }
    }
    // now we can either not access or found a duplicate
    path_comps[i] = NULL;
    for (j=i+1; j<n_comps; j++)
        path_comps[j-1] = path_comps[j];
    n_comps--;
  }


  // assemble everything again
  for (path=opath, i=0;i<n_comps-1;i++)
  {
    mystrcpy(path, path_comps[i]);
    path += strlen(path);
    *path = fePathSep;
    path++;
  }
  if (n_comps)
  {
    mystrcpy(path, path_comps[i]);
  }
  else
  {
    *opath = '\0';
  }
  free(path_comps);
#ifdef RESOURCE_DEBUG
  printf("feCleanUpPath: leaving with path=%s=\n", opath);
#endif
  return opath;
}

// strcpy where source and destination may overlap
static void mystrcpy(char* d, char* s)
{
  /*assume(d != NULL && s != NULL);*/
  while (*s != '\0')
  {
    *d = *s;
    d++;
    s++;
  }
  *d = '\0';
}

/*****************************************************************
 *
 * feSprintf
 *
 *****************************************************************/
static char* feSprintf(char* s, const char* fmt, int warn)
{
  char* s_in = s;
  if (fmt == NULL) return NULL;

  while (*fmt != '\0')
  {
    *s = *fmt;

    if (*fmt == '%' && *(fmt + 1) != '\0')
    {
      fmt++;
      char* r = feResource(*fmt, warn);
      if (r != NULL)
      {
        strcpy(s, r);
        s += strlen(r) - 1;
      }
      else
      {
        s++;
        *s = *fmt;
      }
    }
    else if (*fmt == '$' && *(fmt + 1) != '\0')
    {
      fmt++;
      char* v = s + 1;
      while (*fmt == '_' ||
             (*fmt >= 'A' && *fmt <= 'Z') ||
             (*fmt >= 'a' && *fmt <= 'z'))
      {
        *v = *fmt;
        v++;
        fmt++;
      }
      fmt--;
      *v = '\0';
      v = getenv(s + 1);
      if (v != NULL) strcpy(s, v);
      s += strlen(s) - 1;
    }
    s++;
    fmt++;
  }
  *s = '\0';
  return s_in;
}

