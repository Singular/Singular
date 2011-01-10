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
  /// destroy the object: b points to blackbox_struct, d to data
  void (*blackbox_destroy)(blackbox  *b, void *d);
  /// convert the object to a string (which should be freed by omFree)
  char *(*blackbox_String)(blackbox *b,void *d);
  /// print the object: default: use string representation
  void (*blackbox_Print)(blackbox *b,void *d);
  /// construct the default object
  void *(*blackbox_Init)(blackbox *b);
  /// copy the object: b points to blackbox_struct, d to data
  void *(*blackbox_Copy)(blackbox *b,void *d);
  /// interpreter assign: l:=r
  BOOLEAN (*blackbox_Assign)(leftv l, leftv r);
  /// interpreter: unary operations op(r), r(), ...
  BOOLEAN (*blackbox_Op1)(int op,leftv l, leftv r);
  /// interpreter: binary operations: op(r1,r2), r1 op r2,...
  BOOLEAN (*blackbox_Op2)(int op,leftv l, leftv r1,leftv r2);
  /// interpreter: tertiary op: op(r1,r2,r3)
  BOOLEAN (*blackbox_Op3)(int op,leftv l, leftv r1,leftv r2, leftv r3);
  /// interpreter: operations with undefined number of operands
  BOOLEAN (*blackbox_OpM)(int op,leftv l, leftv r);
  /// additional type info
  void *data;
} ;

/// return the structure to the type given by t
blackbox* getBlackboxStuff(const int t);
/// return the name to the type given by t (r/o)
const char *    getBlackboxName(const int t);
/// used by scanner: returns ROOTDECL for known types (and the type number in t)
int blackboxIsCmd(const char *n, int & tok);
/// define a new type
int setBlackboxStuff(blackbox *bb,const char *name);

/// list all defined type (for debugging)
void printBlackboxTypes();

#endif
