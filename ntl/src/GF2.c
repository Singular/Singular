
#include <NTL/GF2.h>

#include <NTL/new.h>

NTL_START_IMPL


void div(GF2& x, GF2 a, GF2 b)
{
   if (b == 0) Error("GF2: division by zero");
   x = a;
}

void div(GF2& x, long a, GF2 b)
{
   if (b == 0) Error("GF2: division by zero");
   x = a;
}

void div(GF2& x, GF2 a, long b)
{
   if ((b & 1) == 0) Error("GF2: division by zero");
   x = a;
}

void inv(GF2& x, GF2 a)
{
   if (a == 0) Error("GF2: division by zero");
   x = a;
}

void power(GF2& x, GF2 a, long e)
{
   if (e == 0) {
      x = 1;
      return;
   }

   if (e < 0 && a == 0) 
      Error("GF2: division by zero");

   x = a;
}

ostream& operator<<(ostream& s, GF2 a)
{
   if (a == 0)
      s << "0";
   else
      s << "1";

   return s;
}

istream& operator>>(istream& s, GF2& x)
{
   static ZZ a;

   s >> a;
   conv(x, a);
   return s;
}
 
NTL_END_IMPL
