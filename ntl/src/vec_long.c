
#include <NTL/vec_long.h>

#include <NTL/new.h>

NTL_START_IMPL

static inline 
void BlockConstruct(long *, long) { }

static inline 
void BlockDestroy(long *, long) { }

NTL_vector_impl_plain(long,vec_long)

NTL_io_vector_impl(long,vec_long)

NTL_eq_vector_impl(long,vec_long)

NTL_END_IMPL

