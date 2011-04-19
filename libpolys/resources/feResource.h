#ifndef FERESOURCE_H
#define FERESOURCE_H

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <omalloc/mylimits.h>

#define DIR_SEP '/'
#define DIR_SEPP "/"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif


/*****************************************************************
 *
 * Resource management (feResources.cc)
 *
 *****************************************************************/
// returns value of Resource as read-only string, or NULL
// if Resource not found
// issues warning, if explicitely requested (warn > 0), or
// if warn < 0 and Resource is gotten for the first time
// Always quiet if warn == 0
char* feResource(const char id, int warn = -1);
char* feResource(const char* key, int warn = -1);

char* feGetResource(const char id);

// char* feResourceDefault(const char id);
// char* feResourceDefault(const char* key);


// This needs to be called before the first call to feResource
// Initializes Resources, SearchPath, and extends PATH
void feInitResources(char* argv0);
// Re-inits resources, should be called after changing env. variables
void feReInitResources();
// Prints resources into string with StringAppend, etc
void feStringAppendResources(int warn = -1);




const char fePathSep =
// not really cygwin, applies to all windows systems:
#if (defined(CYGWIN) || defined(ix86_Win))
                ';'
#else
                ':'
#endif
                ;



#endif
