#ifndef SY_BIT_H
#define SY_BIT_H
/*
 * **  Set operations (small sets only)
 * */

#define Sy_bit(x)     ((unsigned)1<<(x))
#define Sy_inset(x,s) ((Sy_bit(x)&(s))?TRUE:FALSE)
#define BTEST1(a)     Sy_inset((a), test)
#define BVERBOSE(a)   Sy_inset((a), verbose)

#endif
