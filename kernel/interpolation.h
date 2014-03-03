/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: ideals of points (with multiplicities)
*/

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <vector>

class intvec;
struct sip_sideal; typedef struct sip_sideal * ideal;

ideal interpolation(const std::vector<ideal>& L, intvec *v);


#endif

