#ifndef BLACKBOX_H
#define BLACKBOX_H

#include "kernel/mod2.h"

#include "kernel/structs.h"

#include "Singular/lists.h"
#include "Singular/links/silink.h"

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
  // convention for blackbox_Op1..blackbox_OpM:
  //             return FALSE, if op was successfully performed
  //             return TRUE (and an error message) for failure
  //             return TRUE (and no error message) if not defined
  BOOLEAN (*blackbox_Op1)(int op,leftv l, leftv r);
  /// interpreter: binary operations: op(r1,r2), r1 op r2,...
  BOOLEAN (*blackbox_Op2)(int op,leftv l, leftv r1,leftv r2);
  /// interpreter: tertiary op: op(r1,r2,r3)
  BOOLEAN (*blackbox_Op3)(int op,leftv l, leftv r1,leftv r2, leftv r3);
  /// interpreter: operations with undefined number of operands
  BOOLEAN (*blackbox_OpM)(int op,leftv l, leftv r);
  /// is an assign of r to l (part of b) impossible?
  BOOLEAN (*blackbox_CheckAssign)(blackbox *b,leftv l, leftv r);
  /// serialize
  BOOLEAN (*blackbox_serialize)(blackbox *b,void *d, si_link f);
  /// deserialize
  BOOLEAN (*blackbox_deserialize)(blackbox **b,void **d, si_link f);
  /// additional type info
  void *data;
  /// addtinional gneral properties
  int properties; // bit 0:blackbox is only a wrapper for lists
#define  BB_LIKE_LIST(B) ((B)->properties &1)
} ;
/// default procedure blackboxDefaultOp1, to be called as "default:" branch
BOOLEAN blackboxDefaultOp1(int op,leftv l, leftv r);

/// default procedure blackboxDefaultOp2, to be called as "default:" branch
BOOLEAN blackboxDefaultOp2(int op,leftv l, leftv r1, leftv r2);

/// default procedure blackboxDefaultOp3, to be called as "default:" branch
BOOLEAN blackboxDefaultOp3(int op,leftv l, leftv r1,leftv r2, leftv r3);

/// default procedure blackboxDefaultOpM, to be called as "default:" branch
BOOLEAN blackboxDefaultOpM(int op,leftv l, leftv r);

/// default procedure blackbox_default_Print: print the string
void blackbox_default_Print(blackbox *b,void *d);

/// return the structure to the type given by t
blackbox* getBlackboxStuff(const int t);
/// return the name to the type given by t (r/o)
const char *    getBlackboxName(const int t);
/// used by scanner: returns ROOT_DECL for known types
/// (and the type number in @c tok)
int blackboxIsCmd(const char *n, int & tok);
/// define a new type
int setBlackboxStuff(blackbox *bb,const char *name);

/// list all defined type (for debugging)
void printBlackboxTypes();

#endif
