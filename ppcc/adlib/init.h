#pragma once

extern int ArgC;
extern char **ArgV;
extern Str *ProgName;
extern StrArr *Args;

struct Initializer {
  Initializer *next;
  virtual void init() = 0;
  Initializer() {
    next = NULL;
  }
};

void InitSystem();

extern Initializer *initializers;

#define INIT(name, code) \
  namespace AdLib { namespace Init { \
    struct InitSection_##name : public Initializer { \
      InitSection_##name() \
          : Initializer() { \
        next = initializers; \
        initializers = this; \
      } \
      virtual void init(); \
    } init_section_##name; \
    void InitSection_##name::init() { \
      code \
    } \
  } }
