
#ifndef INCL_CF_GEVAL_H
#define INCL_CF_GEVAL_H
// -*- c++ -*-
//*****************************************************************************
/** @file cfGEval.h
 *
 * @author Martin Lee
 * @date   22.03.2011
 *
 * This file defines a class GEvaluation derived from Evaluation, which
 * generates all points in K^m.
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal
 * @version \$Id$
 *
**/
//*****************************************************************************
#include <config.h>

#include "canonicalform.h"
#include "cf_generator.h"
#include "cf_eval.h"

/** @class GEvaluation
 *
 * GEvaluation is a class derived from @a Evaluation, which can generate all
 * points in K^m, where K is either Z/p, Z/p(alpha) or GF.
 * @sa Evaluation, REvaluation
 */

class GEvaluation: public Evaluation
{
  private:
    /// array to hold the generators
    CFGenerator** gen;
  public:
    /// default constructor
    GEvaluation (): Evaluation(), gen(0) {}

    /// construct an evaluation containing max0-min0 points using a generator
    /// sample of the field K
    GEvaluation (int min0,///<[in] lowest index of array that stores the points
                 int max0,///<[in] highest index of array that stores the points
                 const CFGenerator & sample ///< [in] generator of the ground
                                            ///< field
                );

    /// copy constructor
    GEvaluation (const GEvaluation& e);

    /// destructor
    ~GEvaluation();

    /// assignment
    GEvaluation& operator= (const GEvaluation& e);

    /// get a new point with n non zero components
    /// @return returns false if there is no next point, true otherwise
    bool nextpoint (int n ///<[in] some int
                   );

    /// reset values to zero and reset generators
    void reset();

    /// initialise with n non zero components
    void init (int n ///<[in] should be <= values.max()-values.min()
              );
};

#endif

