#include "kernel/mod2.h"

#ifdef HAVE_POLYMAKE
#include "reporter/reporter.h"

void init_polymake_help()
{
  const char *polymake_banner =
    "Welcome to polymake version\nCopyright (c) 1997-2015\nEwgenij Gawrilow, Michael Joswig (TU Darmstadt)\nhttp://www.polymake.org\n";

  PrintS(polymake_banner);
}
#endif
