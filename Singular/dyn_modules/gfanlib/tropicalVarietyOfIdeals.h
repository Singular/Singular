#ifndef CALLGfANLIB_TROPICALVARIETYOFIDEALS
#define CALLGfANLIB_TROPICALVARIETYOFIDEALS

#include <libpolys/polys/simpleideals.h>
#include <tropicalStrategy.h>

gfan::ZFan* tropicalVariety(const ideal I, const ring r, const tropicalStrategy currentStrategy);

#endif
