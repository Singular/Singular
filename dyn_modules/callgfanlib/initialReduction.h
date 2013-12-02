#ifndef INITIALREDUCTION_H
#define INITIALREDUCTION_H

#include <kernel/structs.h>

#ifndef N_DEBUG
BOOLEAN pReduce(leftv res, leftv args);
BOOLEAN reduceInitially0(leftv res, leftv args);
BOOLEAN reduceInitially1(leftv res, leftv args);
BOOLEAN reduceInitially2(leftv res, leftv args);
#endif

#endif
