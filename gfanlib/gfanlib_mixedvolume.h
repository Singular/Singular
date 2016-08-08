/*
 * gfanlib_mixedvolume.h
 *
 *  Created on: Apr 10, 2016
 *      Author: anders
 */

#ifndef GFANLIB_MIXEDVOLUME_H_
#define GFANLIB_MIXEDVOLUME_H_

#include <vector>

#include "gfanlib_matrix.h"
#include "gfanlib_z.h"

namespace gfan{
        Integer mixedVolume(std::vector<IntMatrix> const &tuple, int nthreads=0, int steps=500);
        namespace MixedVolumeExamples{
                std::vector<IntMatrix> cyclic(int n);
                std::vector<IntMatrix> noon(int n);
                std::vector<IntMatrix> chandra(int n);
                std::vector<IntMatrix> katsura(int n);
                std::vector<IntMatrix> gaukwa(int n);
                std::vector<IntMatrix> eco(int n);
        }

}

#endif /* GFANLIB_MIXEDVOLUME_H_ */
