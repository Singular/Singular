
#include <NTL/tools.h>

#include <ctype.h>

#include <stdio.h>

#include <NTL/new.h>

NTL_START_IMPL


void Error(const char *s)
{
   printf("%s\n", s );
   abort();
}


// The following implementation of CharToIntVal is completely portable.

long CharToIntVal(long a)
{
   switch (a) {
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;

      case 'A': return 10;
      case 'B': return 11;
      case 'C': return 12;
      case 'D': return 13;
      case 'E': return 14;
      case 'F': return 15;

      case 'a': return 10;
      case 'b': return 11;
      case 'c': return 12;
      case 'd': return 13;
      case 'e': return 14;
      case 'f': return 15;

      default:  return -1;
   }
}

// The following implementation of IntValToChar is completely portable.

char IntValToChar(long a)
{
   switch (a) {
      case 0: return '0';
      case 1: return '1';
      case 2: return '2';
      case 3: return '3';
      case 4: return '4';
      case 5: return '5';
      case 6: return '6';
      case 7: return '7';
      case 8: return '8';
      case 9: return '9';

      case 10: return 'a';
      case 11: return 'b';
      case 12: return 'c';
      case 13: return 'd';
      case 14: return 'e';
      case 15: return 'f';

      default: Error("IntValToChar: bad arg");
   }

   return 0;  // to supress warnings
}


long IsWhiteSpace(long a)
{
   if (a > NTL_MAX_INT || a < NTL_MIN_INT)
      return 0;

   int b = (int) a;

   if (isspace(b))
      return 1;
   else 
      return 0;
}

void PrintTime(double t)
{
   long hh, mm, ss;

   ss = long(t + 0.5);

   hh = ss/3600;
   ss = ss - hh*3600;
   mm = ss/60;
   ss = ss - mm*60;

   if (hh > 0)
      printf("%d:",hh);

   if (hh > 0 || mm > 0) {
      if (hh > 0 && mm < 10) printf("0");
      printf("%d:",mm);
   }

   if ((hh > 0 || mm > 0) && ss < 10) printf("0");
   printf("%d:",ss);
}

NTL_END_IMPL
