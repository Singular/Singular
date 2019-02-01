#ifndef FEFOPEN_H
#define FEFOPEN_H

/*****************************************************************
 *
 * File Stuff
 *
 *****************************************************************/
#ifdef __cplusplus

#include <stdio.h>

FILE*feFopen(const char *path, const char *mode, char *where=0, short useWerror=0, short path_only=0);

/*
// These are our versions of fopen and fread They are very similar to
// the usual fopen and fread, except that on reading, they always
// convert "\r\n" into " \n" and "\r" into "\n".
//
// IMPORTANT: do only use myfopen and myfread when reading text,
// do never use fopen and fread
*/
FILE *myfopen(const char *path, const char *mode);
size_t myfread(void *ptr, size_t size, size_t nmemb, FILE *stream);

extern "C"
{
#endif

EXTERN_VAR short errorreported;
void    WerrorS(const char *s);
EXTERN_VAR void (*WerrorS_callback)(const char *s);
EXTERN_VAR void (*PrintS_callback)(const char *s);

#ifdef __cplusplus
}
#endif
#endif
