
#include <NTL/vec_double.h>

#include <NTL/new.h>

NTL_START_IMPL

static inline 
void BlockConstruct(double *, double) { }

static inline 
void BlockDestroy(double *, double) { }

NTL_vector_impl_plain(double,vec_double)

NTL_io_vector_impl(double,vec_double)

NTL_eq_vector_impl(double,vec_double)

NTL_END_IMPL

