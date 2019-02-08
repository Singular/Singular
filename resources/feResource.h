#ifndef FERESOURCE_H
#define FERESOURCE_H

#include <sys/param.h>

#define DIR_SEP '/'
#define DIR_SEPP "/"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#ifdef __cplusplus
#include "../factory/globaldefs.h"
/*****************************************************************
 *
 * Resource management (feResources.cc)
 *
 *****************************************************************/
typedef enum {feResUndef = 0, feResBinary, feResDir, feResFile, feResUrl, feResPath} feResourceType;

typedef struct feResourceConfig_s
{
  const char*           key;   // key to identify resource
  const char            id;    // char id to identify resource
  feResourceType  type;  // type of Resource
  const char*           env;   // env variable to look for
  const char*           fmt;   // format string -- see below for epxlaination
  char*                 value; // what it was set to: may be changed
} feResourceConfig_s;
typedef feResourceConfig_s * feResourceConfig;

EXTERN_VAR feResourceConfig_s feResourceConfigs[];

// returns value of Resource as read-only string, or NULL
// if Resource not found
// issues warning, if explicitely requested (warn > 0), or
// if warn < 0 and Resource is gotten for the first time
// Always quiet if warn == 0
char* feResource(const char id, int warn = -1);
char* feResource(const char* key, int warn = -1);

char* feGetResource(const char id, int warn = -1);

// char* feResourceDefault(const char id);
// char* feResourceDefault(const char* key);


// This needs to be called before the first call to feResource
// Initializes Resources, SearchPath, and extends PATH
void feInitResources(const char* argv0);
// Re-inits resources, should be called after changing env. variables
void feReInitResources();
#endif /* end ifdef __cplusplus */

EXTERN_VAR char* feArgv0;

const char fePathSep = ':' ;

#endif
