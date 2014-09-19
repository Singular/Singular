#ifndef PPINITIALREDUCTION_H
#define PPINITIALREDUCTION_H

#include <kernel/structs.h>
#include <tropicalStrategy.h>

#ifndef NDEBUG
BOOLEAN pppReduce(leftv res, leftv args);
BOOLEAN ppreduceInitially0(leftv res, leftv args);
BOOLEAN ppreduceInitially1(leftv res, leftv args);
BOOLEAN ppreduceInitially2(leftv res, leftv args);
BOOLEAN ppreduceInitially3(leftv res, leftv args);
BOOLEAN ppreduceInitially4(leftv res, leftv args);
#endif

bool isOrderingLocalInT(const ring r);
bool pReduce0(ideal &I, const number p, const ring r);
bool ppreduceInitially(ideal I, const ring r, const number p);
BOOLEAN ppreduceInitially(leftv res, leftv args);

void z_Write(number p, ring r);

#endif
