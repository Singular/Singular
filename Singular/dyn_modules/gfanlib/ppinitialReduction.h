#ifndef PPINITIALREDUCTION_H
#define PPINITIALREDUCTION_H

#include <kernel/structs.h>
#include <tropicalStrategy.h>

typedef std::pair<int,int> mark;
typedef std::vector<std::pair<int,int> > marks;

bool isOrderingLocalInT(const ring r);
void pReduce(ideal &I, const number p, const ring r);
bool ppreduceInitially(ideal I, const ring r, const number p);
/* BOOLEAN ppreduceInitially(leftv res, leftv args); */

#ifndef NDEBUG
BOOLEAN pReduceDebug(leftv res, leftv args);
BOOLEAN reduceInitiallyDebug(leftv res, leftv args);
BOOLEAN ptNormalize(leftv res, leftv args);
BOOLEAN ppreduceInitially3(leftv res, leftv args);
#endif

#endif
