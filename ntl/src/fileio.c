
#include <NTL/fileio.h>

#include <string.h>

#include <NTL/new.h>

NTL_START_IMPL


static char sbuf[256];

char *FileName(const char* stem, const char *ext)
{
   strcpy(sbuf, stem);
   strcat(sbuf, "-");
   strcat(sbuf, ext);

   return sbuf;
}

char *FileName(const char* stem, const char *ext, long d)
{
   strcpy(sbuf, stem);
   strcat(sbuf, "-");
   strcat(sbuf, ext);
   strcat(sbuf, "-");

   char dbuf[6];
   dbuf[5] = '\0';
   long i, dd;
   dd = d;
   for (i = 4; i >= 0; i--) {
      dbuf[i] = IntValToChar(dd % 10);
      dd = dd / 10;
   }

   strcat(sbuf, dbuf);

   return sbuf;
}

NTL_END_IMPL
