// Header file for libfac.a
// Created by -- makeheader --
///////////////////////////////////////
#include <factory.h>

//// Note: second argument for Factorize is for internal use only.
.INCLUDE factor/Factor.h
.INCLUDE factor/MVMultiHensel.h

//// Note: InternalSqrFree does only the work needed for factorization.
.INCLUDE factor/SqrFree.h
.INCLUDE factor/Truefactor.h
.INCLUDE factor/class.h

.INCLUDE factor/helpstuff.h
.INCLUDE factor/tmpl_inst.h

////////////////////////////////////////
/// from charsets:
.INCLUDE charset/csutil.h
.INCLUDE charset/charset.h

.INCLUDE charset/reorder.h
