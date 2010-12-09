#ifndef BIGINTM_H
#define BIGINTM_H
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>

void bigintm_setup();

char * bigintm_String(blackbox *b, void *d);
void * bigintm_Copy(blackbox*b, void *d);
BOOLEAN bigintm_Assign(leftv l, leftv r);
BOOLEAN bigintm_Op2(int op, leftv res, leftv a1, leftv a2);
BOOLEAN bigintm_OpM(int op, leftv res, leftv args);
void bigintm_destroy(blackbox *b, void *d);
#endif
