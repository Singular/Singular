

#include <NTL/config.h>

#if (defined(NTL_STD_CXX) || defined(NTL_PSTD_NHF))
#include <iostream>
#include <fstream>
using namespace std;
#else
#include <iostream.h>
#include <fstream.h>
#endif

int main(int argc, char *argv[])
{
   ifstream f1, f2;

   f1.open(argv[1], ios::in);
   f2.open(argv[2], ios::in);

   double x1, x2;

   f1 >> x1;
   f2 >> x2;

   if (x1 < 0.99*x2)
      return 0;
   else
      return 1;
}
