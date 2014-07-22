#ifndef TTINITIALREDUCTION_H
#define TTINITIALREDUCTION_H

#include <kernel/structs.h>

#ifndef NDEBUG
BOOLEAN ttpReduce(leftv res, leftv args);
BOOLEAN ttreduceInitially0(leftv res, leftv args);
BOOLEAN ttreduceInitially1(leftv res, leftv args);
BOOLEAN ttreduceInitially2(leftv res, leftv args);
BOOLEAN ttreduceInitially3(leftv res, leftv args);
BOOLEAN ttreduceInitially4(leftv res, leftv args);
#endif

BOOLEAN ttreduceInitially(leftv res, leftv args);

#endif
