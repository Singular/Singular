#include "lib.h"

void ContractFailure(ContractType type, const char *message) {
  Str *error = NULL;
  switch (type) {
  case Precondition:
    error = S("precondition failed: ");
    break;
  case Postcondition:
    error = S("postcondition failed: ");
    break;
  case Invariant:
    error = S("invariant failed: ");
    break;
  case Assertion:
    error = S("assrtion failed: ");
    break;
  }
  error->add(S(message));
  ContractException *e = new ContractException();
  e->type = type;
  e->error = error;
  throw e;
}
