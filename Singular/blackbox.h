#ifndef BLACKBOX_H
#define BLACKBOX_H
#include <omalloc.h>
#include <kernel/structs.h>
#include <kernel/febase.h>

void removeBlackboxStuff(const int rt);

struct blackbox_struct;

typedef struct blackbox_struct blackbox;

struct  blackbox_struct
{
  void (*blackbox_destroy)(void  *b, void *d);
  char *(*blackbox_String)(blackbox *b,void *d);
  void (*blackbox_Print)(blackbox *b,void *d);
  void *(*blackbox_Init)();
  void *(*blackbox_Copy)(blackbox *b,void *d);
  BOOLEAN (*blackbox_Assign)(leftv l, leftv r);
  BOOLEAN (*blackbox_Op1)(int op,leftv l, leftv r);
  BOOLEAN (*blackbox_Op2)(int op,leftv l, leftv r1,leftv r2);
  BOOLEAN (*blackbox_Op3)(int op,leftv l, leftv r1,leftv r2, leftv r3);
  BOOLEAN (*blackbox_OpM)(int op,leftv l, leftv r);
} ;

blackbox* getBlackboxStuff(const int t);
const char *    getBlackboxName(const int t);
int blackboxIsCmd(const char *n, int & tok);
int setBlackboxStuff(blackbox *bb,char *name);

void * loadNewBlackbox(char *type_name,const char *id_name);
void printBlackboxTypes();

#endif
