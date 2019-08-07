/*
 *
 */

#ifndef _TYPMAP_H
#define _TYPMAP_H

/* MYYSTYPE avoids including of Singular's struct.h, which is not needed. */
#define MYYSTYPE
#include <Singular/tok.h>
#undef MYYSTYPE

THREAD_VAR extern char type_conv[MAX_TOK][32];
void init_type_conv() ;


#endif /* _TYPMAP_H */
