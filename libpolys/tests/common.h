#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <iostream>
#include <fstream>
#include <string.h>

#include "cxxtest/TestSuite.h"
#include "cxxtest/GlobalFixture.h"

#include "misc/auxiliary.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "reporter/reporter.h"
#include "resources/feResource.h"

#ifndef PLURAL_INTERNAL_DECLARATIONS
#define PLURAL_INTERNAL_DECLARATIONS
#endif

#include "polys/nc/gb_hack.h"

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
      default: return o << "Unknown type: [" << (unsigned long) type << "]";
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

    std::stringstream ss;
    {
      char* s = StringEndS();  ss << s; omFree(s);
    }

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
      strncpy(s + ll, ".log", 5);
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

      StringSetS("resources in use (as reported by feStringAppendResources(0):\n");
      feStringAppendResources(0);

      { char* s = StringEndS(); PrintS(s); omFree(s); }

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


template void CxxTest::doAssertDiffers<n_Procs_s*, void*>(char const*, unsigned int, char const*, n_Procs_s*, char const*, void*, char const*);
template void CxxTest::doAssertDiffers<snumber* (*)(long, n_Procs_s*), void*>(char const*, unsigned int, char const*, snumber* (*)(long, n_Procs_s*), char const*, void*, char const*);
template void CxxTest::doAssertDiffers<snumber* (*)(snumber*, snumber*, n_Procs_s*), void*>(char const*, unsigned int, char const*, snumber* (*)(snumber*, snumber*, n_Procs_s*), char const*, void*, char const*);
template void CxxTest::doAssertDiffers<void (*)(n_Procs_s*, int), void*>(char const*, unsigned int, char const*, void (*)(n_Procs_s*, int), char const*, void*, char const*);
template void CxxTest::doAssertDiffers<void (*)(snumber**, n_Procs_s*), void*>(char const*, unsigned int, char const*, void (*)(snumber**, n_Procs_s*), char const*, void*, char const*);
template void CxxTest::doAssertDiffers<void (*)(snumber*&, n_Procs_s*), void*>(char const*, unsigned int, char const*, void (*)(snumber*&, n_Procs_s*), char const*, void*, char const*);
template void CxxTest::doAssertEquals<int, int>(char const*, unsigned int, char const*, int, char const*, int, char const*);
template void CxxTest::doAssertEquals<n_coeffType, n_coeffType>(char const*, unsigned int, char const*, n_coeffType, char const*, n_coeffType, char const*);
template void CxxTest::doAssertEquals<snumber* (*)(long, n_Procs_s*), snumber* (*)(long, n_Procs_s*)>(char const*, unsigned int, char const*, snumber* (*)(long, n_Procs_s*), char const*, snumber* (*)(long, n_Procs_s*), char const*);
template void CxxTest::doAssertEquals<snumber* (*)(snumber*, snumber*, n_Procs_s*), snumber* (*)(snumber*, snumber*, n_Procs_s*)>(char const*, unsigned int, char const*, snumber* (*)(snumber*, snumber*, n_Procs_s*), char const*, snumber* (*)(snumber*, snumber*, n_Procs_s*), char const*);
template void CxxTest::doAssertEquals<void (*)(snumber**, n_Procs_s*), void (*)(snumber**, n_Procs_s*)>(char const*, unsigned int, char const*, void (*)(snumber**, n_Procs_s*), char const*, void (*)(snumber**, n_Procs_s*), char const*);

template bool CxxTest::differs<n_Procs_s*, void*>(n_Procs_s*, void*);
template bool CxxTest::differs<snumber* (*)(long, n_Procs_s*), void*>(snumber* (*)(long, n_Procs_s*), void*);
template bool CxxTest::differs<snumber* (*)(snumber*, snumber*, n_Procs_s*), void*>(snumber* (*)(snumber*, snumber*, n_Procs_s*), void*);
template bool CxxTest::differs<void (*)(n_Procs_s*, int), void*>(void (*)(n_Procs_s*, int), void*);
template bool CxxTest::differs<void (*)(snumber**, n_Procs_s*), void*>(void (*)(snumber**, n_Procs_s*), void*);
template bool CxxTest::differs<void (*)(snumber*&, n_Procs_s*), void*>(void (*)(snumber*&, n_Procs_s*), void*);
template bool CxxTest::equals<int, int>(int, int);
template bool CxxTest::equals<n_coeffType, n_coeffType>(n_coeffType, n_coeffType);
template bool CxxTest::equals<snumber* (*)(long, n_Procs_s*), snumber* (*)(long, n_Procs_s*)>(snumber* (*)(long, n_Procs_s*), snumber* (*)(long, n_Procs_s*));
template bool CxxTest::equals<snumber* (*)(snumber*, snumber*, n_Procs_s*), snumber* (*)(snumber*, snumber*, n_Procs_s*)>(snumber* (*)(snumber*, snumber*, n_Procs_s*), snumber* (*)(snumber*, snumber*, n_Procs_s*));
template bool CxxTest::equals<void (*)(snumber**, n_Procs_s*), void (*)(snumber**, n_Procs_s*)>(void (*)(snumber**, n_Procs_s*), void (*)(snumber**, n_Procs_s*));
template char* CxxTest::numberToString<long>(long, char*, long, unsigned int, unsigned int);

template void CxxTest::doAssertDiffers<ip_sring*, void*>(char const*, unsigned int, char const*, ip_sring*, char const*, void*, char const*);
template void CxxTest::doAssertEquals<short, int>(char const*, unsigned int, char const*, short, char const*, int, char const*);

template bool CxxTest::differs<ip_sring*, void*>(ip_sring*, void*);
template bool CxxTest::equals<short, int>(short, int);

#endif /* TESTS_COMMON_H */
