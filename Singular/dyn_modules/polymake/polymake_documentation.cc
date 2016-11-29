#include <kernel/mod2.h>

#ifdef HAVE_POLYMAKE

/*
#include <polymake_conversion.h>

#include <Singular/dyn_modules/gfanlib/bbcone.h>
#include <Singular/dyn_modules/gfanlib/bbfan.h>
#include <Singular/dyn_modules/gfanlib/bbpolytope.h>

#include <Singular/blackbox.h>
#include <Singular/ipshell.h>
#include <Singular/subexpr.h>

#include <string>
*/

#include <Singular/ipid.h>


void init_polymake_help()
{
  const char *polymake_banner =
    "Welcome to polymake version\nCopyright (c) 1997-2015\nEwgenij Gawrilow, Michael Joswig (TU Darmstadt)\nhttp://www.polymake.org\n";

  PrintS(polymake_banner);
}
#endif
