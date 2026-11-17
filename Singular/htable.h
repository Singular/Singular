#ifndef HTABLE_H
#define HTABLE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include "kernel/mod2.h"

#include "Singular/subexpr.h"

struct stelem;
typedef struct stelem *   telem;

struct stelem
{
  telem next;
  char* key;
  sleftv val;
  uint32_t hash;
  int origin_typ;
  void *origin_data;
  ring val_ring;
};

struct stablerec
{
  telem *t;
  int max;
  int ref;
  int count;
};
#define TABLE_MAX 1000

stablerec* t_createTable(int s);
void t_destroyTable(stablerec* t);
stablerec* copyTable(stablerec* t);
stablerec* t_cloneTable(stablerec* t);
char* stringTable(stablerec* t);
void htable_Print(stablerec *d);
int t_countTable(stablerec *t);
BOOLEAN t_RingDependend(stablerec *t);
/// find the entry to key s
telem t_findTable(stablerec* t,const char *s);
/// find the data to key s
leftv t_findTabelVal(stablerec* t,const char *s);
/// copy the data at key s to res
BOOLEAN t_copyTableVal(leftv res, stablerec* t,const char *s);
/// add a new entry (key s, data v) to table t; eats s, copies v.
/// duplicate keys are errors unless the stored and new values have identical
/// type and data pointer.
BOOLEAN t_addTable(stablerec* t,char *s, leftv v);
/// add a new entry while preserving the original source pointer used for
/// duplicate detection; eats s, copies v.
BOOLEAN t_addTableOrigin(stablerec* t,char *s, leftv v, int origin_typ, void *origin_data);
/// merge src into dst; values are copied, duplicate semantics as above.
BOOLEAN t_mergeTable(stablerec* dst, stablerec* src);
#endif
