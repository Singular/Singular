#ifndef PPINITIALREDUCTION_H
#define PPINITIALREDUCTION_H

#include <kernel/structs.h>

#ifndef NDEBUG
BOOLEAN pppReduce(leftv res, leftv args);
BOOLEAN ppreduceInitially0(leftv res, leftv args);
BOOLEAN ppreduceInitially1(leftv res, leftv args);
BOOLEAN ppreduceInitially2(leftv res, leftv args);
BOOLEAN ppreduceInitially3(leftv res, leftv args);
BOOLEAN ppreduceInitially4(leftv res, leftv args);
#endif

bool ppreduceInitially(ideal I, ring r);
BOOLEAN ppreduceInitially(leftv res, leftv args);

#endif
