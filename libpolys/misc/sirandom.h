#ifndef SIRANDOM_H
#define SIRANDOM_H
#ifdef __cplusplus
extern "C" {
#endif
EXTERN_VAR int siSeed;
int siRand(void);

typedef int (*siRandProc)(void);
typedef int (*siRandProc1)(int);
int siRandNext(int);
int siRandPlus1(int);


#ifdef __cplusplus
}
#endif

#endif
//
