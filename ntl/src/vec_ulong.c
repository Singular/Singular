
#include <NTL/vec_ulong.h>

#include <NTL/new.h>

NTL_START_IMPL

static inline 
void BlockConstruct(_ntl_ulong *, long) { }

static inline 
void BlockDestroy(_ntl_ulong *, long) { }

NTL_vector_impl_plain(_ntl_ulong,vec_ulong)

NTL_io_vector_impl(_ntl_ulong,vec_ulong)

NTL_eq_vector_impl(_ntl_ulong,vec_ulong)

NTL_END_IMPL

