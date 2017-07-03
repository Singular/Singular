#ifndef INTERVAL_H
#define INTERVAL_H

#include "Singular/ipid.h"

struct interval
{
    number lower;
    number upper;
    ring R;

    interval();
    interval(number);
    interval(number, number);
    interval(interval*);
    ~interval();
};

struct box
{
    interval** intervals;
    ring R;

    box();
    box(box*);
    ~box();

    box& setInterval(int, interval*);
};

//extern int intervalID;
//extern int boxID;

// helpful functions
//interval* intervalScalarMultiply(number, interval*);
//interval* intervalMultiply(interval*, interval*);
//interval* intervalAdd(interval*, interval*);
//interval* intervalSubtract(interval*, interval*);
//bool intervalEqual(interval*, interval*);
//bool intervalContainsZero(interval*);

extern "C" int mod_init(SModulFunctions*);

#endif
/* INTERVAL_H */
