#ifndef PPINITIALREDUCTION_H
#define PPINITIALREDUCTION_H

#include <kernel/structs.h>
#include <tropicalStrategy.h>

typedef std::pair<int,int> mark;
typedef std::vector<std::pair<int,int> > marks;

bool isOrderingLocalInT(const ring r);
void pReduce(ideal &I, const number p, const ring r);
void pReduceInhomogeneous(poly &g, const number p, const ring r);
bool ppreduceInitially(ideal I, const ring r, const number p);
/* BOOLEAN ppreduceInitially(leftv res, leftv args); */

#endif
