
#include <NTL/ctools.h>

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

   cout << "\n\n";
   cout << "Basic Configuration Options:\n";


#ifdef NTL_STD_CXX
   cout << "NTL_STD_CXX\n";
#endif

#ifdef NTL_PSTD_NNS
   cout << "NTL_PSTD_NNS\n";
#endif

#ifdef NTL_PSTD_NHF
   cout << "NTL_PSTD_NHF\n";
#endif

#ifdef NTL_PSTD_NTN
   cout << "NTL_PSTD_NTN\n";
#endif

#ifdef NTL_GMP_LIP
   cout << "NTL_GMP_LIP\n";
#endif

#ifdef NTL_GMP_HACK
   cout << "NTL_GMP_HACK\n";
#endif

#ifdef NTL_LONG_LONG_TYPE
   cout << "NTL_LONG_LONG_TYPE: ";
   cout << make_string(NTL_LONG_LONG_TYPE) << "\n";
#endif

#ifdef NTL_UNSIGNED_LONG_LONG_TYPE
   cout << "NTL_UNSIGNED_LONG_LONG_TYPE: ";
   cout << make_string(NTL_UNSIGNED_LONG_LONG_TYPE) << "\n";
#endif

#ifdef NTL_CXX_ONLY
   cout << "NTL_CXX_ONLY\n";
#endif


#ifdef NTL_X86_FIX
   cout << "NTL_X86_FIX\n";
#endif

#ifdef NTL_NO_X86_FIX
   cout << "NTL_NO_X86_FIX\n";
#endif

#ifdef NTL_NO_INIT_TRANS
   cout << "NTL_NO_INIT_TRANS\n";
#endif

#ifdef NTL_CLEAN_INT
   cout << "NTL_CLEAN_INT\n";
#endif

#ifdef NTL_CLEAN_PTR
   cout << "NTL_CLEAN_PTR\n";
#endif

#ifdef NTL_RANGE_CHECK
   cout << "NTL_RANGE_CHECK\n";
#endif


cout << "\n";
cout << "Resolution of double-word types:\n";
cout << make_string(NTL_LL_TYPE) << "\n";
cout << make_string(NTL_ULL_TYPE) << "\n";


cout << "\n";
cout << "Performance Options:\n";

#ifdef NTL_LONG_LONG
   cout << "NTL_LONG_LONG\n";
#endif

#ifdef NTL_AVOID_FLOAT
   cout << "NTL_AVOID_FLOAT\n";
#endif

#ifdef NTL_SPMM_UL
   cout << "NTL_SPMM_UL\n";
#endif


#ifdef NTL_SPMM_ULL
   cout << "NTL_SPMM_ULL\n";
#endif


#ifdef NTL_SPMM_ASM
   cout << "NTL_SPMM_ASM\n";
#endif




#ifdef NTL_AVOID_BRANCHING
   cout << "NTL_AVOID_BRANCHING\n";
#endif



#ifdef NTL_TBL_REM
   cout << "NTL_TBL_REM\n";
#endif


#ifdef NTL_GF2X_ALTCODE
   cout << "NTL_GF2X_ALTCODE\n";
#endif


#ifdef NTL_GF2X_ALTCODE1
   cout << "NTL_GF2X_ALTCODE1\n";
#endif


#ifdef NTL_GF2X_NOINLINE
   cout << "NTL_GF2X_NOINLINE\n";
#endif

   cout << "\n\n";

   return 0;
}
