#ifndef GFANLIB_ADJUSTWEIGHTS_H
#define GFANLIB_ADJUSTWEIGHTS_H

#include <gfanlib/gfanlib_vector.h>

gfan::ZVector nonvalued_adjustWeightForHomogeneity(const gfan::ZVector w);
gfan::ZVector valued_adjustWeightForHomogeneity(const gfan::ZVector w);
gfan::ZVector nonvalued_adjustWeightUnderHomogeneity(const gfan::ZVector e, const gfan::ZVector w);
gfan::ZVector valued_adjustWeightUnderHomogeneity(const gfan::ZVector e, const gfan::ZVector w);

#endif
