
#include <NTL/config.h>

#if (defined(NTL_STD_CXX) || defined(NTL_PSTD_NHF))
#include <iostream>
#include <fstream>
using namespace std;
#else
#include <iostream.h>
#include <fstream.h>
#endif

#define make_string_aux(x) #x
#define make_string(x) make_string_aux(x)



int main()
{

#ifdef NTL_STD_CXX
   cout << "s/<NTL_STD_CXX>/1/\n";
#else
   cout << "s/<NTL_STD_CXX>/0/\n";
#endif

#ifdef NTL_PSTD_NNS
   cout << "s/<NTL_PSTD_NNS>/1/\n";
#else
   cout << "s/<NTL_PSTD_NNS>/0/\n";
#endif

#ifdef NTL_PSTD_NHF
   cout << "s/<NTL_PSTD_NHF>/1/\n";
#else
   cout << "s/<NTL_PSTD_NHF>/0/\n";
#endif

#ifdef NTL_PSTD_NTN
   cout << "s/<NTL_PSTD_NTN>/1/\n";
#else
   cout << "s/<NTL_PSTD_NTN>/0/\n";
#endif

#ifdef NTL_GMP_LIP
   cout << "s/<NTL_GMP_LIP>/1/\n";
#else
   cout << "s/<NTL_GMP_LIP>/0/\n";
#endif

#ifdef NTL_GMP_HACK
   cout << "s/<NTL_GMP_HACK>/1/\n";
#else
   cout << "s/<NTL_GMP_HACK>/0/\n";
#endif

#ifdef NTL_LONG_LONG_TYPE
   cout << "s/<NTL_LONG_LONG_TYPE>/1/\n";
   cout << "s/<VAL_NTL_LONG_LONG_TYPE>/" make_string(NTL_LONG_LONG_TYPE) "/\n";
#else
   cout << "s/<NTL_LONG_LONG_TYPE>/0/\n";
   cout << "s/<VAL_NTL_LONG_LONG_TYPE>/long long/\n";
#endif

#ifdef NTL_CXX_ONLY
   cout << "s/<NTL_CXX_ONLY>/1/\n";
#else
   cout << "s/<NTL_CXX_ONLY>/0/\n";
#endif


#ifdef NTL_X86_FIX
   cout << "s/<NTL_X86_FIX>/1/\n";
#else
   cout << "s/<NTL_X86_FIX>/0/\n";
#endif

#ifdef NTL_NO_X86_FIX
   cout << "s/<NTL_NO_X86_FIX>/1/\n";
#else
   cout << "s/<NTL_NO_X86_FIX>/0/\n";
#endif


#ifdef NTL_NO_INIT_TRANS
   cout << "s/<NTL_NO_INIT_TRANS>/1/\n";
#else
   cout << "s/<NTL_NO_INIT_TRANS>/0/\n";
#endif

#ifdef NTL_RANGE_CHECK
   cout << "s/<NTL_RANGE_CHECK>/1/\n";
#else
   cout << "s/<NTL_RANGE_CHECK>/0/\n";
#endif



   ofstream s;

#ifdef NTL_GMP_LIP
   s.open("GMP_LIP", ios::out);
#else
   s.open("NO_GMP_LIP", ios::out);
#endif

   return 0;
}
