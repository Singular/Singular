
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

#ifdef NTL_RANGE_CHECK
   cout << "NTL_RANGE_CHECK\n";
#endif

cout << "\n";
cout << "Performance Options:\n";

#ifdef NTL_LONG_LONG
   cout << "NTL_LONG_LONG\n";
#endif

#ifdef NTL_AVOID_FLOAT
   cout << "NTL_AVOID_FLOAT\n";
#endif

#ifdef NTL_AVOID_BRANCHING
   cout << "NTL_AVOID_BRANCHING\n";
#endif

#ifdef NTL_FFT_PIPELINE
   cout << "NTL_FFT_PIPELINE\n";
#endif


#ifdef NTL_TBL_REM
   cout << "NTL_TBL_REM\n";
#endif

   cout << "\n\n";

   return 0;
}
