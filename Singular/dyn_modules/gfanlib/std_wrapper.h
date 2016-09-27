#ifndef CALLGFANLIB_STD_WRAPPER_H
#define CALLGFANLIB_STD_WRAPPER_H

#include <polys/simpleideals.h>
#include <kernel/structs.h>

ideal gfanlib_kStd_wrapper(ideal I, ring r, tHomog h=testHomog);
ideal gfanlib_satStd_wrapper(ideal I, ring r, tHomog h=testHomog);
ideal gfanlib_monomialabortStd_wrapper(ideal I, ring r, tHomog h=testHomog);

#endif
