// Header file for libfac.a
// Created by -- makeheader --
///////////////////////////////////////
#ifndef FACTOR_H
#define FACTOR_H

#include <factory/factory.h>

.INCLUDE factor/interrupt.h


// some values you can ask for:
/*
extern const char * libfac_name;
extern const char * libfac_version;
extern const char * libfac_date;
extern const char * libfac_author;
*/

//// Note: second argument for Factorize is for internal use only.
.INCLUDE factor/Factor.h
.INCLUDE factor/MVMultiHensel.h

//// Note: InternalSqrFree does only the work needed for factorization.
.INCLUDE factor/SqrFree.h
.INCLUDE factor/Truefactor.h
.INCLUDE factor/class.h

.INCLUDE factor/helpstuff.h
.INCLUDE factor/tmpl_inst.h
.INCLUDE factor/homogfactor.h
.INCLUDE factor/debug.h
.INCLUDE factor/timing.h

////////////////////////////////////////
/// from charsets:
.INCLUDE charset/csutil.h
.INCLUDE charset/charset.h

.INCLUDE charset/reorder.h

.INCLUDE charset/algfactor.h

#endif /* FACTOR_H */
