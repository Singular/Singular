
#include <NTL/pair_ZZX_long.h>

#include <NTL/new.h>

NTL_START_IMPL

NTL_pair_impl(ZZX,long,pair_ZZX_long)
NTL_pair_io_impl(ZZX,long,pair_ZZX_long)
NTL_pair_eq_impl(ZZX,long,pair_ZZX_long)

NTL_vector_impl(pair_ZZX_long,vec_pair_ZZX_long)
NTL_io_vector_impl(pair_ZZX_long,vec_pair_ZZX_long)
NTL_eq_vector_impl(pair_ZZX_long,vec_pair_ZZX_long)

NTL_END_IMPL
