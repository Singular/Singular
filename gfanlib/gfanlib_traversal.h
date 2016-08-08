#ifndef GFANLIB_TRAVERSAL_H_INCLUDED
#define GFANLIB_TRAVERSAL_H_INCLUDED

#include "gfanlib_zfan.h"

namespace gfan{
/*
 This file contains the generic algorithm for traversing a connected component of a pure fan up to symmetry.
 */
class FanTraverser
{
public:
        /**
         * Go to the cone which is connected to the current facet through the ridge in direction ray.
         * The "ridge" is a relative interior point of the ridge.
         */
        virtual void changeCone(ZVector const &ridgeVector, ZVector const &rayVector)=0;
/**
 * Compute the link of the fan in the ridge given by the vector ridge IS THIS A FACET NORMAL OR AN INTERIOR POINT?
 * This gives a list of symmetry invariant points under the actions keeping the link fixed.
 */
        virtual std::list<ZVector> link(ZVector const &ridgeVector)=0;
        virtual ZCone & refToPolyhedralCone()=0;
/**
 * If there is no cone state data for the traverser, half of the changeCone() calls can be avoided.
 * That this is a valid of optimization for the ConeTraverser is indicated returning true in the following function.
 */
        virtual bool hasNoState()const;
};

class Target
{
public:
        virtual bool process(FanTraverser &traverser)=0;
};

class FanBuilder : public Target
{
        ZFan coneCollection;
public:
        ZFan const &getFanRef(){return coneCollection;}
        FanBuilder(int n, SymmetryGroup const &sym);
        bool process(FanTraverser &Traverser);
};

void traverse(FanTraverser &traverser, Target &target, SymmetryGroup const *sym=0);
};
#endif
