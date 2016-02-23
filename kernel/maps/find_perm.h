#ifndef FIND_PERM_H
#define FIND_PERM_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
#include <coeffs/coeffs.h>
#include <polys/matpol.h>
/// helper function for maMapIdeal
/// mapping ideal/matrix/module for the case of a permutation:
/// maps the ideal/module/matrix to_map via the map from preimage_r with image image
/// and mapping of coefficients via nMap (or return NULL)
matrix ma_ApplyPermForMap(const matrix to_map, const ring preimage_r, const ideal image,
  const ring image_r, const nMapFunc nMap);
#endif


