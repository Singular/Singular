#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <iostream>
#include <fstream>
#include <string.h>

#include <cxxtest/TestSuite.h>
#include <cxxtest/GlobalFixture.h>

#include "config.h"

#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>

#include <reporter/reporter.h>
#include <findexec/feResource.h>

#ifndef PLURAL_INTERNAL_DECLARATIONS
#define PLURAL_INTERNAL_DECLARATIONS
#endif

#ifndef PLURAL_INTERNAL_DECLARATIONS_GB_HACK
#define PLURAL_INTERNAL_DECLARATIONS_GB_HACK
#endif

#include <polys/nc/gb_hack.h>

#ifdef HAVE_FACTORY
int initializeGMP(){ return 1; } // due to Factory...
int mmInit(void) {return 1; } // ? due to SINGULAR!!!...???
#endif

// #pragma GCC diagnostic ignored "-Wwrite-strings"
namespace
{
  static inline std::ostream& operator<< (std::ostream& o, const n_coeffType& type)
  {
#define CASE(A) case A: return o << (" " # A) << " ";
    switch( type )
    {
      CASE(n_unknown);
      CASE(n_Zp);
      CASE(n_Q);
      CASE(n_R);
      CASE(n_GF);
      CASE(n_long_R);
      CASE(n_algExt);
      CASE(n_transExt);
      CASE(n_long_C);
      CASE(n_Z);
      CASE(n_Zn);
      CASE(n_Znm);
      CASE(n_Z2m);
      CASE(n_CF);
      default: return o << "Unknown type: [" << (const unsigned long) type << "]";
    }
#undef CASE
    return o;
  }

  template<typename T>
      static inline std::string _2S(T i)
  {
    std::stringstream ss;
    ss << i;
//    std::string s = ss.str();
    return ss.str();
  }


  static inline std::string _2S(number a, const coeffs r)
  {
    n_Test(a,r);
    StringSetS("");
    n_Write(a, r);

    const char* s = StringAppendS("");

    std::stringstream ss;  ss << s;

    StringSetS("");
    return ss.str();

  }

  static inline void PrintSized(/*const*/ number a, const coeffs r, BOOLEAN eoln = TRUE)
  {
    std::clog << _2S(a, r) << ", of size: " << n_Size(a, r);

    if( eoln )
      std::clog << std::endl;
  }



}

class GlobalPrintingFixture : public CxxTest::GlobalFixture
{
   std::ofstream _ofs;
   bool _redirect;
  public:
    GlobalPrintingFixture(bool redirect = false): _redirect(redirect){}

    ~GlobalPrintingFixture()
    {
      if( _ofs)
        _ofs.close();
    }

    void Redirect()
    {
      const int ll = strlen(argv0);
      const int l = 5 + ll;
      char* s = (char *)omAlloc0(l);
      s = strncpy(s, argv0, ll);
      strncpy(s + ll, ".log", 4);
      _ofs.open(s); // , ios_base::out)
      omFreeSize((ADDRESS)s, l);

      std::clog.rdbuf(_ofs.rdbuf());
    }

    virtual bool setUpWorld()
    {
      if( _redirect )
        Redirect();

      std::clog << std::endl << ( "<world>" ) << std::endl << std::endl;
      feInitResources(argv0);

      StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
      feStringAppendResources(0);
      PrintS(StringAppendS("\n"));

      return true;
    }

    virtual bool tearDownWorld()
    {
        std::clog << std::endl << std::endl <<( "</world>" )  << std::endl  << std::endl ;
        return true;
    }
    virtual bool setUp() { std::clog << std::endl << std::endl <<( "<test>" ) << std::endl  << std::endl; return true; }
    virtual bool tearDown() { std::clog << std::endl << std::endl <<( "</test>" ) << std::endl  << std::endl; return true; }
};


#endif /* TESTS_COMMON_H */
