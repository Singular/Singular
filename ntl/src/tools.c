
#include <NTL/tools.h>

#include <NTL/new.h>

NTL_START_IMPL


void Error(const char *s)
{
   cerr << s << "\n";
   abort();
}

long SkipWhiteSpace(istream& s)
{
   long c;

   c = s.peek();
   while (c == ' ' || c == '\n' || c == '\t') {
      s.get();
      c = s.peek();
   }

   if (c == EOF)
      return 0;
   else
      return 1;
}


void PrintTime(ostream& s, double t)
{
   long hh, mm, ss;

   ss = long(t + 0.5);

   hh = ss/3600;
   ss = ss - hh*3600;
   mm = ss/60;
   ss = ss - mm*60;

   if (hh > 0)
      s << hh << ":";

   if (hh > 0 || mm > 0) {
      if (hh > 0 && mm < 10) s << "0";
      s << mm << ":";
   }

   if ((hh > 0 || mm > 0) && ss < 10) s << "0";
   s << ss;
}

NTL_END_IMPL
