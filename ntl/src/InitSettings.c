
#include <NTL/config.h>

#if (defined(NTL_STD_CXX) || defined(NTL_PSTD_NHF))
#include <iostream>
using namespace std;
#else
#include <iostream.h>
#endif

#define make_string_aux(x) #x
#define make_string(x) make_string_aux(x)



int main()
{

#ifdef NTL_STD_CXX
   cout << "NTL_STD_CXX=1\n";
#else
   cout << "NTL_STD_CXX=0\n";
#endif

#ifdef NTL_PSTD_NNS
   cout << "NTL_PSTD_NNS=1\n";
#else
   cout << "NTL_PSTD_NNS=0\n";
#endif

#ifdef NTL_PSTD_NHF
   cout << "NTL_PSTD_NHF=1\n";
#else
   cout << "NTL_PSTD_NHF=0\n";
#endif

#ifdef NTL_PSTD_NTN
   cout << "NTL_PSTD_NTN=1\n";
#else
   cout << "NTL_PSTD_NTN=0\n";
#endif

#ifdef NTL_GMP_LIP
   cout << "NTL_GMP_LIP=1\n";
#else
   cout << "NTL_GMP_LIP=0\n";
#endif

#ifdef NTL_GMP_HACK
   cout << "NTL_GMP_HACK=1\n";
#else
   cout << "NTL_GMP_HACK=0\n";
#endif

#ifdef NTL_GF2X_LIB
   cout << "NTL_GF2X_LIB=1\n";
#else
   cout << "NTL_GF2X_LIB=0\n";
#endif

#ifdef NTL_LONG_LONG_TYPE
   cout << "FLAG_LONG_LONG_TYPE=1\n";
   cout << "NTL_LONG_LONG_TYPE=" make_string(NTL_LONG_LONG_TYPE) "\n";
#else
   cout << "FLAG_LONG_LONG_TYPE=0\n";
   cout << "NTL_LONG_LONG_TYPE=long long\n";
#endif


#ifdef NTL_UNSIGNED_LONG_LONG_TYPE
   cout << "FLAG_UNSIGNED_LONG_LONG_TYPE=1\n";
   cout << "NTL_UNSIGNED_LONG_LONG_TYPE=" make_string(NTL_UNSIGNED_LONG_LONG_TYPE) "\n";
#else
   cout << "FLAG_UNSIGNED_LONG_LONG_TYPE=0\n";
   cout << "NTL_UNSIGNED_LONG_LONG_TYPE=unsigned long long\n";
#endif

#ifdef NTL_CXX_ONLY
   cout << "NTL_CXX_ONLY=1\n";
#else
   cout << "NTL_CXX_ONLY=0\n";
#endif


#ifdef NTL_X86_FIX
   cout << "NTL_X86_FIX=1\n";
#else
   cout << "NTL_X86_FIX=0\n";
#endif

#ifdef NTL_NO_X86_FIX
   cout << "NTL_NO_X86_FIX=1\n";
#else
   cout << "NTL_NO_X86_FIX=0\n";
#endif


#ifdef NTL_NO_INIT_TRANS
   cout << "NTL_NO_INIT_TRANS=1\n";
#else
   cout << "NTL_NO_INIT_TRANS=0\n";
#endif

#ifdef NTL_CLEAN_INT
   cout << "NTL_CLEAN_INT=1\n";
#else
   cout << "NTL_CLEAN_INT=0\n";
#endif

#ifdef NTL_CLEAN_PTR
   cout << "NTL_CLEAN_PTR=1\n";
#else
   cout << "NTL_CLEAN_PTR=0\n";
#endif

#ifdef NTL_RANGE_CHECK
   cout << "NTL_RANGE_CHECK=1\n";
#else
   cout << "NTL_RANGE_CHECK=0\n";
#endif


   return 0;
}
