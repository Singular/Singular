#include <stdio.h>

#include <NTL/tools.h>

#include <NTL/new.h>

NTL_START_IMPL


void Error(const char *s)
{
   printf( "%s\n",s);
   abort();
}

NTL_END_IMPL
