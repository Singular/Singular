#ifndef GEN_MAPS_H
#define GEN_MAPS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/// polynomial map for ideals/module/matrix
/// map_id: the ideal to map
/// map_r: the base ring for map_id
/// image_id: the image of the variables
/// image_r: the base ring for image_id
/// nMap: map for coeffcients
ideal maMapIdeal(const ideal map_id, const ring map_r,const ideal image_id, const ring image_r, const nMapFunc nMap);

/// polynomial map for poly (vector)
/// map_p: the poly (vector) to map
/// map_r: the base ring for map_p
/// image_id: the image of the variables
/// image_r: the base ring for image_id
/// nMap: map for coeffcients
poly maMapPoly(const poly map_p, const ring map_r,const ideal image_id, const ring image_r, const nMapFunc nMap);

poly p_SubstPoly (poly p, int var, poly image, const ring preimage_r, const ring image_r, const nMapFunc nMap, matrix cache=NULL);
ideal id_SubstPoly (ideal id, int var, poly image, const ring preimage_r, const ring image_r, const nMapFunc nMap);

#endif

