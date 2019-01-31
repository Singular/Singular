#pragma once

enum ContractType {
  Precondition,
  Postcondition,
  Invariant,
  Assertion,
};

void ContractFailure(ContractType type, const char *message);

class Str;

struct ContractException : public GC {
  ContractType type;
  Str *error;
};

#if ADLIB_DEBUG
static inline void require(bool cond, const char *message) {
  if (!cond)
    ContractFailure(Precondition, message);
}

static inline void ensure(bool cond, const char *message) {
  if (!cond)
    ContractFailure(Postcondition, message);
}

static inline void invariant(bool cond, const char *message) {
  if (!cond)
    ContractFailure(Invariant, message);
}

static inline void assert(bool cond, const char *message) {
  if (!cond)
    ContractFailure(Assertion, message);
}
#else
static inline void require(bool cond, const char *message) {
}

static inline void ensure(bool cond, const char *message) {
}

static inline void invariant(bool cond, const char *message) {
}

static inline void assert(bool cond, const char *message) {
}
#endif
